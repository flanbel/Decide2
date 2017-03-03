#include "Transform.h"
#include "GameObject.h"

Transform::~Transform()
{
	//親が居るなら
	if(_Parent)
	{
		//親の子から自分を外す
		_Parent->RemoveChild(this);
	}
	//子がいるなら
	if(_Children.size() > 0)
	{
		//子の親をunullに
		vector<Transform*>::iterator it = _Children.begin();
		while (it != _Children.end())
		{
			(*it)->_Parent = nullptr;
			it++;
		}
	}
}

void Transform::Awake()
{
	_Parent = nullptr;
	_Children.clear();

	position = localPosition = angle = localAngle = Vector3::zero;
	scale = localScale = Vector3::one;

	D3DXMatrixIdentity(&_WorldMatrix);
	D3DXMatrixIdentity(&_RotateMatrix);
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
Transform* Transform::GetParent()
{
	return _Parent;
};

//引数のトランスフォームを自分の親に登録する
//第一引数：Transform
void Transform::SetParent(Transform* _Parent)
{
	//親に登録
	this->_Parent = _Parent;
	//親の子に自分を登録
	_Parent->_Children.push_back(this);
}

//子供を検索する関数
//戻り値：Transform* ヒットした子のアドレス
//第一引数：char* 子供の名前
Transform* Transform::FindChild(char* childname)
{
	////for eachはコピーしているので返しても意味ない？
	//for each (Transform* t in _Children)
	//{
	//	if(t->gameObject->Name() == childname)
	//	{
	//		return t;
	//	}
	//}

	//イテレータ取得
	vector<Transform*>::iterator it = _Children.begin();
	//最後まで見る
	while (it != _Children.end())
	{
		//ヒットしたか？
		if((*it)->gameObject->Name() == childname)
		{
			return (*it);
		}
		it++;
	}

	return nullptr;
}
Transform * Transform::FindChild(int idx)
{
	//範囲外アクセスをはじく
	if (_Children.size() <= idx)
	{
		return nullptr;
	}
	//添え字番目を返す
	return _Children[idx];
}
//未実装
Transform ** Transform::FindChilds(char * childname)
{
	vector<Transform*> v;
	//イテレータ取得
	vector<Transform*>::iterator it = _Children.begin();
	//最後まで見る
	while (it != _Children.end())
	{
		//ヒットしたか？
		if ((*it)->gameObject->Name() == childname)
		{
			
		}
		it++;
	}

	return nullptr;
}
;

//子の数を返す関数
//戻り値：int 子の数
int Transform::ChildCnt()
{
	return _Children.size();
};

//子供たち取得
vector<Transform*> Transform::Children()
{
	return _Children;
};

//ローカルからトランスフォームを更新
//親から更新していくべき
//戻り値：なし
//引数：なし
void Transform::UpdateTransform()
{
	//親が居るなら
	if (_Parent) {
		//親がいる。
		D3DXMATRIX PWorld = _Parent->WorldMatrix();
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
		scale.x = localScale.x * _Parent->scale.x;
		scale.y = localScale.y * _Parent->scale.y;
		scale.z = localScale.z * _Parent->scale.z;
		//クォータニオン　未実装
		angle.x = localAngle.x;
		angle.y = localAngle.y;
		angle.z = localAngle.z;
		//angle = localAngle * qParentRot;
		D3DXQUATERNION q;
		D3DXQuaternionRotationYawPitchRoll(&q, D3DXToRadian(angle.y), D3DXToRadian(angle.x), D3DXToRadian(angle.z));
		rotation = q;
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
	D3DXMatrixIdentity(&_RotateMatrix);
	D3DXQUATERNION q;
	rotation.CopyFrom(q);
	D3DXMatrixRotationQuaternion(&_RotateMatrix, &q);

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
	_WorldMatrix = Scale * _RotateMatrix * Pos;
}

void Transform::LockAt(GameObject * obj)
{
	D3DXVECTOR3 target, me;
	obj->transform->position.CopyFrom(target);
	transform->position.CopyFrom(me);
	D3DXMatrixIdentity(&_RotateMatrix);
	//ターゲットから見た自分
	D3DXMatrixLookAtLH(&_RotateMatrix, &target, &me, &D3DXVECTOR3(0, 1, 0));
	//逆行列に
	D3DXMatrixInverse(&_RotateMatrix, NULL, &_RotateMatrix);
	_RotateMatrix._41 = 0.0f;   // オフセットを切る（回転行列だけにしてしまう）
	_RotateMatrix._42 = 0.0f;
	_RotateMatrix._43 = 0.0f;

	//ワールド行列を求める。
	UpdateWolrdMatrix();
}

//トランスフォーム解放
void Transform::Release()
{
	vector<Transform*>::iterator it = _Children.begin();
	while (it != _Children.end())
	{
		//親を削除
		(*it)->_Parent = nullptr;
		it++;
	}
	_Children.clear();
}

void Transform::RemoveChild(Transform * t)
{
	vector<Transform*>::iterator it = _Children.begin();
	while (it != _Children.end())
	{
		//アドレス比較
		if((*it) == t)
		{
			(*it)->_Parent = nullptr;
			_Children.erase(it);
			return;
		}
		it++;
	}
}

//////以下セッター・ゲッター


//ゲッター
D3DXMATRIX Transform::WorldMatrix()
{
	return _WorldMatrix;
}


//セッター
void Transform::WorldMatrix(D3DXMATRIX w)
{
	_WorldMatrix = w;
}

//ゲッター
D3DXMATRIX Transform::RotateMatrix()
{
	return _RotateMatrix;
}

//ゲッター
D3DXMATRIX* Transform::RotateMatrixAddress ()
{
	return &_RotateMatrix;
}