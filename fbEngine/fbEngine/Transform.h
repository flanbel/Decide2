#pragma once
#include "Component.h"

//トランスフォームクラス
class Transform : public Component
{
public:
	Transform(GameObject* g, Transform* t) : Component(g, this)
	{
		name = (char*)typeid(*this).name();
	}
	~Transform();

	void Awake()override;
	void Start()override;
	void Update()override;

	//親を返す関数
	//戻り値：Transform* 親のアドレス
	Transform* GetParent();

	//自分の親に登録する
	//第一引数：Transform 親
	void SetParent(Transform* parent);

	//子供を検索する関数
	//戻り値：Transform* 一番最初にヒットした子のアドレス
	//第一引数：char* 子供の名前
	Transform* FindChild(char* childname);
	//添え字で取得
	Transform* FindChild(int idx);
	//子供を検索する関数
	//戻り値：Transform** ヒットした子達のアドレスの配列
	//第一引数：char* 子供の名前
	Transform** FindChilds(char* childname);

	//子の数を返す関数
	//戻り値：int 子の数
	int ChildCnt();

	//子供たち取得
	vector<Transform*> Children();

	//ローカルからトランスフォームを更新
	//戻り値：なし
	//引数：なし
	void UpdateTransform();

	//トランスフォームからワールド行列更新
	//戻り値：なし
	//引数：なし
	void UpdateWolrdMatrix();

	//受け取ったベクトルをこいつから見た向きに変換
	Vector3 Direction(Vector3 v)
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
	//こいつから見たローカルな位置に変換(回転込み)
	Vector3 Local(Vector3 v)
	{
		D3DXVECTOR4 pos;
		D3DXVECTOR3 lpos;
		v.CopyFrom(lpos);
		D3DXVec3Transform(&pos, &lpos, &_WorldMatrix);
		return Vector3(pos.x, pos.y, pos.z);
	}
	Vector3 LocalPos(Vector3 v)
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
	//受け取ったトランスフォームの向きを見る
	void LockAt(GameObject* obj);

	//解放
	void Release();

	void RemoveChild(Transform* t);

	//ゲッター
	D3DXMATRIX WorldMatrix();
	void WorldMatrix(D3DXMATRIX);

	//ゲッター
	D3DXMATRIX RotateMatrix();
	D3DXMATRIX* RotateMatrixAddress();

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