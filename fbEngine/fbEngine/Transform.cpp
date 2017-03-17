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

//子供を検索する関数
//戻り値：Transform* ヒットした子のアドレス
//第一引数：char* 子供の名前
Transform* Transform::FindChild(char* childname)
{
	//イテレータ取得
	vector<Transform*>::iterator it = _Children.begin();
	//最後まで見る
	while (it != _Children.end())
	{
		//ヒットしたか？
		if((*it)->gameObject->GetName() == childname)
		{
			return (*it);
		}
		it++;
	}

	return nullptr;
}
Transform * Transform::FindChild(unsigned int idx)
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
		if ((*it)->gameObject->GetName() == childname)
		{
			
		}
		it++;
	}

	return nullptr;
}

//ローカルからトランスフォームを更新
//親から更新していくべき
//戻り値：なし
//引数：なし
void Transform::UpdateTransform()
{
	//親が居るなら
	if (_Parent) {
		//親がいる。
		D3DXMATRIX PWorld = _Parent->GetWorldMatrix();
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

Vector3 Transform::Direction(Vector3 v)
{
	D3DXVECTOR3 in;
	D3DXVECTOR3 out;
	v.CopyFrom(in);
	D3DXVec3TransformCoord(&out, &in, &_RotateMatrix);

	Vector3 r = out;
	//正規化はどうしようか？？？
	//r.Normalize();
	return r;
}

Vector3 Transform::Local(Vector3 v)
{
	D3DXVECTOR4 pos;	//ポジション
	D3DXVECTOR3 lpos;	//ローカルポジション
	//vをコピー
	v.CopyFrom(lpos);
	//ワールド行列から見たローカルポジションをposに格納
	D3DXVec3Transform(&pos, &lpos, &_WorldMatrix);
	return Vector3(pos.x, pos.y, pos.z);
}

Vector3 Transform::LocalPos(Vector3 v)
{
	D3DXVECTOR4 pos;
	D3DXVECTOR3 lpos;
	v.CopyFrom(lpos);
	D3DXMATRIX offset;
	D3DXMatrixIdentity(&offset);
	offset._41 = _WorldMatrix._41;
	offset._42 = _WorldMatrix._42;
	offset._43 = _WorldMatrix._43;
	D3DXVec3Transform(&pos, &lpos, &offset);
	return Vector3(pos.x, pos.y, pos.z);
}

void Transform::LockAt(GameObject * obj)
{
	D3DXVECTOR3 target, me;
	obj->transform->position.CopyFrom(target);
	this->position.CopyFrom(me);
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