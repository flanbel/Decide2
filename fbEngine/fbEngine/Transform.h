#pragma once
#include "Component.h"

//トランスフォームクラス
class Transform : public Component
{
public:
	Transform(GameObject* g, Transform* t) :
		Component(g, t, typeid(this).name())
	{
		
	}
	~Transform();

	void Awake()override;
	void Start()override;
	void Update()override;

	//子供を検索する関数
	//戻り値：Transform* 一番最初にヒットした子のアドレス
	//第一引数：char* 子供の名前
	Transform* FindChild(char* childname);
	//添え字で取得
	Transform* FindChild(unsigned int idx);
	//子供を検索する関数(未実装)
	//戻り値：Transform** ヒットした子達のアドレスの配列
	//第一引数：char* 子供の名前
	Transform** FindChilds(char* childname);

	//ローカルからトランスフォームを更新
	void UpdateTransform();

	//トランスフォームからワールド行列更新
	void UpdateWolrdMatrix();

	//受け取ったベクトルをこいつから見た向きに変換
	Vector3 Direction(Vector3 v);
	//ローカルな位置に変換(回転込み)
	Vector3 Local(Vector3 v);
	//ローカルな位置に変換(回転抜き)
	Vector3 LocalPos(Vector3 v);
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
	}

	//子供たち取得
	const vector<Transform*>& GetChildren()
	{
		return _Children;
	};

	//ゲッター
	const D3DXMATRIX& Transform::GetWorldMatrix()
	{
		return _WorldMatrix;
	}

	//セッター
	void Transform::SetWorldMatrix(D3DXMATRIX w)
	{
		_WorldMatrix = w;
	}

	//ゲッター
	const D3DXMATRIX& Transform::RotateMatrix()
	{
		return _RotateMatrix;
	}

	//ゲッター
	D3DXMATRIX* Transform::RotateMatrixAddress()
	{
		return &_RotateMatrix;
	}


	//簡単にアクセスしたかった・・・。
	Vector3	position;		//最終ポジション
	Vector3	localPosition;	//ローカルポジション
	Vector3	scale;			//最終スケール
	Vector3	localScale;		//ローカルスケール
	Vector3	angle;			//回転(度)
	Vector3	localAngle;		//ローカル回転
	Quaternion rotation;	//回転（クウォータニオン）
private:
	Transform* _Parent;		//親のアドレス
	vector<Transform*> _Children;		//子供達のアドレスを格納した
	
	D3DXMATRIX _RotateMatrix;	//回転行列
	D3DXMATRIX _WorldMatrix;	//ワールド行列
};