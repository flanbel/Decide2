#include "Transform.h"
#include "GameObject.h"

Transform::~Transform()
{
	//親が居るなら
	if(parent)
	{
		//親の子から自分を外す
		parent->RemoveChild(this);
	}
	//子がいるなら
	if(children.size() > 0)
	{
		//子の親をunullに
		map<char*, Transform*>::iterator it = children.begin();
		while (it != children.end())
		{
			it->second->parent = nullptr;
			it++;
		}
	}
}

void Transform::Awake()
{
	parent = nullptr;
	children.clear();

	position = localPosition = angle = localAngle = Vector3::zero;
	scale = localScale = Vector3::one;

	D3DXMatrixIdentity(&worldMatrix);
	D3DXMatrixIdentity(&rotateMatrix);
}

void Transform::Start()
{
	UpdateTransform();
}

void Transform::Update()
{
	UpdateTransform();
}

//親を返す関数
//戻り値：Transform* 親のアドレス
Transform* Transform::Parent()
{
	return parent;
};

//引数のトランスフォームを自分の親に登録する
//戻り値：bool 登録できたか否か
//第一引数：Transform
bool Transform::SetParent(Transform* parent)
{
	//子に同じ名前がいないか検索
	auto child = parent->children.find(gameObject->Name());
	//ヒットしなかった。
	if (child == parent->children.end())
	{
		//親に登録
		this->parent = parent;
		//親の子に自分を登録
		parent->children.insert(make_pair(gameObject->Name(), this));

		return true;
	}
	//ヒットしてしまった。
	else
	{
		////登録できなかった旨を表示
		////場所
		//string place;
		////行数
		//string line = "?";
		////Support::IntToString(&line, __LINE__);
		////ファイル名
		//string file = __FILE__;
		//place = "ファイル名："+ file + '\n' +"行数：" + line + "行"+ "でエラーが発生しました。\n";
		//string log = this->gameObject->Name() + "は同じ名前の子が既に登録されていたので" + parent->gameObject->name() + "に登録できませんでした。";
		//log = place + log;
		//MessageBox(NULL, log.c_str(), TEXT("エラー"), MB_OK);
		return false;
	}
}

//子供を検索する関数
//戻り値：Transform* ヒットした子のアドレス
//第一引数：char* 子供の名前
Transform* Transform::FindChild(char* childname)
{
	//子の検索
	auto child = children.find(childname);
	//ヒットした
	if (child != children.end())
	{
		//子を返す
		return child->second;
	}
	//ヒットしなかった
	else
	{
		return nullptr;
	}
};

//子の数を返す関数
//戻り値：int 子の数
int Transform::ChildCnt()
{
	return children.size();
};

//子供たち取得
map<char*, Transform*> Transform::Children()
{
	return children;
};

//ローカルからトランスフォームを更新
//親から更新していくべき
//戻り値：なし
//引数：なし
void Transform::UpdateTransform()
{
	//親が居るなら
	if (parent) {
		//親がいる。
		D3DXMATRIX PWorld = parent->WorldMatrix();
		//親のワールド行列を乗算して、ローカル座標をワールド座標に変換する。
		D3DXVECTOR4 pos;
		D3DXVECTOR3 lpos;
		//ローカルをコピー
		localPosition.CopyFrom(lpos);
		//ローカル×ワールド
		D3DXVec3Transform(&pos, &lpos, &PWorld);
		//ポジション
		position.x = pos.x;
		position.y = pos.y;
		position.z = pos.z;
		//スケール値
		scale.x = localScale.x * parent->scale.x;
		scale.y = localScale.y * parent->scale.y;
		scale.z = localScale.z * parent->scale.z;
		//クォータニオン　未実装
		angle.x = localAngle.x;
		angle.y = localAngle.y;
		angle.z = localAngle.z;
		//angle = localAngle * qParentRot;
	}
	else {
		//ローカルをそのまま
		position = localPosition;
		scale = localScale;
		angle = localAngle;
		D3DXQUATERNION q;
		D3DXQuaternionRotationYawPitchRoll(&q, D3DXToRadian(angle.y), D3DXToRadian(angle.x), D3DXToRadian(angle.z));
		rotation = q;
	}
	//回転
	D3DXMatrixIdentity(&rotateMatrix);
	D3DXQUATERNION q;
	rotation.CopyFrom(q);
	D3DXMatrixRotationQuaternion(&rotateMatrix, &q);

	UpdateWolrdMatrix();
}

//トランスフォームからワールド行列更新
//戻り値：なし
//引数：なし
void Transform::UpdateWolrdMatrix()
{
	//ワールド行列を求める。
	D3DXMATRIX Scale, Pos;
	D3DXMatrixIdentity(&Scale);
	D3DXMatrixIdentity(&Pos);
	//サイズ
	D3DXMatrixScaling(&Scale, scale.x, scale.y, scale.z);
	//ポジション
	D3DXMatrixTranslation(&Pos, position.x, position.y, position.z);
	worldMatrix = Scale * rotateMatrix * Pos;
}

void Transform::LockAt(GameObject * obj)
{
	D3DXVECTOR3 target, me;
	obj->transform->position.CopyFrom(target);
	transform->position.CopyFrom(me);
	D3DXMatrixIdentity(&rotateMatrix);
	//ターゲットから見た自分
	D3DXMatrixLookAtLH(&rotateMatrix, &target, &me, &D3DXVECTOR3(0, 1, 0));
	//逆行列に
	D3DXMatrixInverse(&rotateMatrix, NULL, &rotateMatrix);
	rotateMatrix._41 = 0.0f;   // オフセットを切る（回転行列だけにしてしまう）
	rotateMatrix._42 = 0.0f;
	rotateMatrix._43 = 0.0f;

	//ワールド行列を求める。
	UpdateWolrdMatrix();
}

//トランスフォーム解放
void Transform::Release()
{
	map<char*, Transform*>::iterator it = children.begin();
	while (it != children.end())
	{
		//親を削除
		it->second->parent = nullptr;
		it++;
	}
	children.clear();
}

void Transform::RemoveChild(Transform * t)
{
	map<char*, Transform*>::iterator it = children.begin();
	while (it != children.end())
	{
		//アドレス比較
		if(it->second == t)
		{
			it->second->parent = nullptr;
			children.erase(it);
			return;
		}
		it++;
	}
}

//////以下セッター・ゲッター


//ゲッター
D3DXMATRIX Transform::WorldMatrix()
{
	return worldMatrix;
}


//セッター
void Transform::WorldMatrix(D3DXMATRIX w)
{
	worldMatrix = w;
}

//ゲッター
D3DXMATRIX Transform::RotateMatrix()
{
	return rotateMatrix;
}

//ゲッター
D3DXMATRIX* Transform::RotateMatrixAddress ()
{
	return &rotateMatrix;
}