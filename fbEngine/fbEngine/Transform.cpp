#include "Transform.h"
#include "GameObject.h"

Transform::Transform(GameObject * g, Transform * t) :
	Component(g, t, typeid(this).name()),
	_Parent(nullptr),
	_LocalPosition(Vector3::zero),
	_Position(Vector3::zero),
	_LocalScale(Vector3::one),
	_Scale(Vector3::one),
	_LocalAngle(Vector3::zero),
	_Angle(Vector3::zero),
	_LocalRotation(Quaternion::Identity),
	_Rotation(Quaternion::Identity)
{
	_Children.clear();
	D3DXMatrixIdentity(&_RotateMatrix);
	D3DXMatrixIdentity(&_WorldMatrix);
}

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
		//子の親をnullに
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
	
}

void Transform::Start()
{
	
}

void Transform::Update()
{
	//UpdateTransform();
}

//子供を検索する関数
//戻り値：Transform* ヒットした子のアドレス
//第一引数：char* 子供の名前
Transform* Transform::FindChild(char* name)
{
	//イテレータ取得
	vector<Transform*>::iterator it = _Children.begin();
	//最後まで見る
	while (it != _Children.end())
	{
		//子の名前取得
		const char* childname = (*it)->gameObject->GetName();
		//名前比較
		if(strcmp(childname,name) == 0)
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
Transform ** Transform::FindChilds(char * name)
{
	vector<Transform*> v;
	//イテレータ取得
	vector<Transform*>::iterator it = _Children.begin();
	//最後まで見る
	while (it != _Children.end())
	{
		//子の名前取得
		const char* childname = (*it)->gameObject->GetName();
		//名前比較
		if (strcmp(childname, name) == 0)
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
		_LocalPosition.CopyFrom(lpos);
		//ローカル×ワールド
		D3DXVec3Transform(&pos, &lpos, &PWorld);
		//ポジション
		_Position.x = pos.x;
		_Position.y = pos.y;
		_Position.z = pos.z;
		//スケール値
		_Scale = _LocalScale * _Parent->_Scale;
		//オイラー角
		Vector3 pang = _Parent->GetAngle();
		_Angle = _LocalAngle + pang;
		//クォータニオン生成
		_Rotation.SetEuler(_Angle);
	}
	else {
		//ローカルをそのまま
		_Position = _LocalPosition;
		_Scale = _LocalScale;
		_Angle = _LocalAngle;
		_Rotation.SetEuler(_Angle);
	}
	//回転行列取得
	_RotateMatrix = _Rotation.GetRotationMatrix();

	UpdateWolrdMatrix();
}

//トランスフォームからワールド行列更新
//戻り値：なし
//引数：なし
void Transform::UpdateWolrdMatrix()
{
	//初期化
	D3DXMATRIX Scale, Pos;
	D3DXMatrixIdentity(&Scale);
	D3DXMatrixIdentity(&Pos);
	//スケール
	D3DXMatrixScaling(&Scale, _Scale.x, _Scale.y, _Scale.z);
	//ポジション
	D3DXMatrixTranslation(&Pos, _Position.x, _Position.y, _Position.z);
	//ワールド行列設定
	_WorldMatrix = Scale * _RotateMatrix * Pos;

	//子も更新する
	for each (Transform* t in _Children)
	{
		t->UpdateTransform();
	}
}

Vector3 Transform::GetForward()
{
	return Direction(Vector3::front);
}

Vector3 Transform::Direction(const Vector3& v)
{
	D3DXVECTOR3 in;
	D3DXVECTOR3 out;
	v.CopyFrom(in);
	//受け取ったベクトルを回転行列を使って回転させる。
	D3DXVec3TransformCoord(&out, &in, &_RotateMatrix);

	Vector3 r = out;
	return r;
}

Vector3 Transform::Local(const Vector3& v)
{
	D3DXVECTOR4 pos;	//ポジション
	D3DXVECTOR3 lpos;	//ローカルポジション
	//vをコピー
	v.CopyFrom(lpos);
	//ワールド行列から見たローカルポジションをposに格納
	//ワールド行列を使って移動させる。
	D3DXVec3Transform(&pos, &lpos, &_WorldMatrix);
	return Vector3(pos.x, pos.y, pos.z);
}

Vector3 Transform::LocalPos(const Vector3& v)
{
	D3DXVECTOR4 pos;
	D3DXVECTOR3 lpos;
	v.CopyFrom(lpos);
	D3DXMATRIX offset;
	D3DXMatrixIdentity(&offset);
	offset._41 = _WorldMatrix._41;
	offset._42 = _WorldMatrix._42;
	offset._43 = _WorldMatrix._43;
	//移動行列を使って移動させる
	D3DXVec3Transform(&pos, &lpos, &offset);
	return Vector3(pos.x, pos.y, pos.z);
}

void Transform::LockAt(GameObject * obj)
{
	//注視点と視点
	D3DXVECTOR3 target, eye;
	obj->transform->_Position.CopyFrom(target);
	this->_Position.CopyFrom(eye);
	//回転行列初期化
	D3DXMatrixIdentity(&_RotateMatrix);
	//視点から見た上方向取得
	Vector3 vup = this->Direction(Vector3::up);
	//ターゲットから見た自分
	//第四引数は視点の上方向
	D3DXMatrixLookAtLH(&_RotateMatrix, &target, &eye, (D3DXVECTOR3*)&vup);
	//ビュー行列を逆行列にしてワールド行列に
	D3DXMatrixInverse(&_RotateMatrix, NULL, &_RotateMatrix);
	// オフセットを切って回転行列だけにしてしまう
	_RotateMatrix._41 = 0.0f;   
	_RotateMatrix._42 = 0.0f;
	_RotateMatrix._43 = 0.0f;

	//回転行列からクォータニオン生成
	D3DXQUATERNION q;
	D3DXQuaternionRotationMatrix(&q, &_RotateMatrix);
	//メンバ変数のクォータニオンにコピー
	_Rotation = q;
	//クォータニオンからオイラー角を求める
	SetAngle(_Rotation.GetAngle());
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//セッター・ゲッター

void Transform::SetPosition(const Vector3 & v)
{
	_Position = v;
	//最終的なポジションからローカルを逆算
	if (_Parent)
	{
		Vector3 ppos = _Parent->GetPosition();
		_LocalPosition = _Position - ppos;
	}
	else
	{
		//そのままローカルに
		_LocalPosition = _Position;
	}
	//ワールド行列更新
	UpdateWolrdMatrix();
}

void Transform::SetLocalPosition(const Vector3& v)
{
	_LocalPosition = v;

	//ローカルから最終的なポジションを計算
	if (_Parent)
	{
		D3DXMATRIX pworld = _Parent->GetWorldMatrix();

		D3DXVECTOR3 lpos;
		D3DXVECTOR4 pos;
		//ローカルをコピー
		_LocalPosition.CopyFrom(lpos);
		//親のワールド行列を乗算して、ローカル座標をワールド座標に変換する。
		D3DXVec3Transform(&pos, &lpos, &pworld);
		//ポジション
		_Position.x = pos.x;
		_Position.y = pos.y;
		_Position.z = pos.z;
	}
	else
	{
		//ローカルそのまま
		_Position = _LocalPosition;
	}
	//ワールド行列更新
	UpdateWolrdMatrix();
}

void Transform::SetLocalScale(const Vector3& v)
{
	_LocalScale = v;
	//ローカルから最終的なスケール計算
	if (_Parent) {
		//ローカルスケール×親のスケール
		_Scale = _LocalScale * _Parent->_Scale;
	}
	else
	{
		//ローカルそのまま
		_Scale = _LocalScale;
	}
	//ワールド行列更新
	UpdateWolrdMatrix();
}

void Transform::SetScale(const Vector3& v)
{
	_Scale = v;
	//最終的なスケールからローカルを逆算
	if (_Parent)
	{
		//ローカルスケールを求める
		_LocalScale = _Scale / _Parent->_Scale;
	}
	else
	{
		//そのままローカルに
		_LocalScale = _Scale;
	}
	//ワールド行列更新
	UpdateWolrdMatrix();
}

void Transform::SetLocalAngle(const Vector3& v)
{
	_LocalAngle = v;
	//ローカルから最終的な角度計算
	if (_Parent) {
		//親のクォータニオン取得
		Quaternion prot = _Parent->GetRotation();
		//各軸の回転量を取得
		Vector3 pang = prot.GetAngle();
		//足す
		_Angle = _LocalAngle + pang;
	}
	else
	{
		//ローカルそのまま
		_Angle = _LocalAngle;
	}
	//クォータニオン更新
	_LocalRotation.SetEuler(_Angle);
	//クォータニオン設定
	SetLocalRotation(_LocalRotation);
}

void Transform::SetAngle(const Vector3& v)
{
	_Angle = v;
	//ローカルから最終的な角度計算
	if (_Parent) {
		//親のクォータニオン取得
		Quaternion prot = _Parent->GetRotation();
		//各軸の回転量を取得
		Vector3 pang = prot.GetAngle();
		//引く
		_LocalAngle = _Angle - pang;
	}
	else
	{
		//ローカルそのまま
		_LocalAngle = _Angle;
	}
	//クォータニオン更新
	_LocalRotation.SetEuler(_Angle);
	//クォータニオン設定
	SetLocalRotation(_LocalRotation);
}

void Transform::SetLocalRotation(const Quaternion & q)
{
	_LocalRotation = q;
	//親がいるかどうか？
	if (_Parent) {
		//親のクォータニオン取得
		Quaternion protInv = _Parent->GetRotation();
		//逆クォータニオンにする。
		protInv.Inverse();
		//ローカルと掛ける
		_Rotation = _LocalRotation * protInv;
	}
	else
	{
		//ローカルそのまま
		_Rotation = _LocalRotation;
	}
	//回転行列更新
	_RotateMatrix = _Rotation.GetRotationMatrix();
	//ワールド行列更新
	UpdateWolrdMatrix();
}

void Transform::SetRotation(const Quaternion& q)
{
	_Rotation = q;
	//親がいるかどうか？
	if (_Parent) {
		//親のクォータニオン取得
		Quaternion protInv = _Parent->GetRotation();
		//逆クォータニオンにする。
		protInv.Inverse();
		//ローカルと掛ける
		_LocalRotation = _Rotation / protInv;
	}
	else
	{
		//ローカルそのまま
		_LocalRotation = _Rotation;
	}
	//回転行列更新
	_RotateMatrix = _Rotation.GetRotationMatrix();
	//ワールド行列更新
	UpdateWolrdMatrix();
}

void Transform::SetWorldMatrix(D3DXMATRIX w)
{
	_WorldMatrix = w;
}
//ゲッター
Vector3& Transform::GetLocalPosition()
{
	return _LocalPosition;
}

const Vector3& Transform::GetPosition()
{
	return _Position;
}

const Vector3& Transform::GetLocalScale()
{
	return _LocalScale;
}

const Vector3& Transform::GetScale()
{
	return _Scale;
}

const Vector3& Transform::GetLocalAngle()
{
	return _LocalAngle;
}

const Vector3& Transform::GetAngle()
{
	return _Angle;
}

const Quaternion& Transform::GetLocalRotation()
{
	return _LocalRotation;
}

const Quaternion& Transform::GetRotation()
{
	return _Rotation;
}

const D3DXMATRIX & Transform::GetWorldMatrix()
{
	return _WorldMatrix;
}

const D3DXMATRIX & Transform::GetRotateMatrix()
{
	return _RotateMatrix;
}

D3DXMATRIX * Transform::GetRotateMatrixAddress()
{
	return &_RotateMatrix;
}