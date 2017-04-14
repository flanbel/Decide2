#pragma once
#include "Component.h"

class Camera;

//トランスフォームクラス
class Transform : public Component
{
public:
	Transform(GameObject* g, Transform* t);
	~Transform();

	void Awake()override;
	void Start()override;
	void Update()override;

	//子供を検索する関数
	//戻り値：Transform* 一番最初にヒットした子のアドレス
	//第一引数：char* 子供の名前
	Transform* FindChild(char* name);
	//添え字で取得
	Transform* FindChild(unsigned int idx);
	//子供を検索する関数(未実装)
	//戻り値：Transform** ヒットした子達のアドレスの配列
	//第一引数：char* 子供の名前
	Transform** FindChilds(char* name);

	//ローカルからトランスフォームを更新
	void UpdateTransform();

	//トランスフォームからワールド行列更新
	void UpdateWolrdMatrix();

	//前向きのベクトル取得
	Vector3 GetForward();
	//受け取ったベクトルをこいつから見た向きに変換
	Vector3 Direction(const Vector3& v);
	//ローカルな位置に変換(回転込み)
	Vector3 Local(const Vector3& v);
	//ローカルな位置に変換(回転抜き)
	Vector3 LocalPos(const Vector3& v);
	//受け取ったオブジェクトの向きを見る
	void LockAt(GameObject* obj);

	//解放
	void Release();
	//子を外す
	void RemoveChild(Transform* t);

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
		UpdateTransform();
	}

	//子供たち取得
	const vector<Transform*>& GetChildren()
	{
		return _Children;
	};

	//各セッター・ゲッター

	void SetLocalPosition(const Vector3& v);
	void SetPosition(const Vector3& v);
	void SetLocalScale(const Vector3& v);
	void SetScale(const Vector3& v);
	void SetLocalAngle(const Vector3& v);
	void SetAngle(const Vector3& v);
	void SetLocalRotation(const Quaternion& q);
	void SetRotation(const Quaternion& q);
	void SetWorldMatrix(D3DXMATRIX w);

	Vector3&	GetLocalPosition();
	const Vector3& GetPosition();
	const Vector3& GetLocalScale();
	const Vector3& GetScale();
	const Vector3&	GetLocalAngle();
	const Vector3& GetAngle();
	const Quaternion& GetLocalRotation();
	const Quaternion& GetRotation();
	D3DXMATRIX* GetRotateMatrixAddress();
	const D3DXMATRIX& GetRotateMatrix();
	const D3DXMATRIX& GetWorldMatrix();
private:
	Transform* _Parent;		//親のアドレス
	vector<Transform*> _Children;		//子供達のアドレスを格納した
	
	Vector3	_Position;		//最終ポジション
	Vector3	_LocalPosition;	//ローカルポジション
	Vector3	_Scale;			//最終スケール
	Vector3	_LocalScale;	//ローカルスケール
	Vector3	_Angle;			//角度
	Vector3	_LocalAngle;	//ローカル角度
	Quaternion _Rotation;		//回転（クウォータニオン）
	Quaternion _LocalRotation;	//ローカル回転（クウォータニオン）
	D3DXMATRIX _RotateMatrix;	//回転行列
	D3DXMATRIX _WorldMatrix;	//ワールド行列
};