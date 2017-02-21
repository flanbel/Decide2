#pragma once
#include "Component.h"

//方向ライト
struct DirectionalLight
{
public:
	Vector3 rot;	//回転
	Color color;	//色
	DirectionalLight()
	{
		rot = Vector3::zero;
		color = Color::white;
	}
	//方向を返す
	Vector4 Direction() const
	{
		//奥方向ベクトル
		D3DXVECTOR3 in = { 0.0f,0.0f,1.0f };
		D3DXVECTOR3 out;
		//回転行列作成
		D3DXMATRIX rotateMatrix;
		D3DXMatrixRotationYawPitchRoll(&rotateMatrix, D3DXToRadian(rot.y), D3DXToRadian(rot.x), D3DXToRadian(rot.z));
		//ベクトルを回転
		D3DXVec3TransformCoord(&out, &in, &rotateMatrix);
		Vector3 vec3;
		vec3.Set(out);
		vec3.Normalize();
		Vector4 r = Vector4(vec3.x, vec3.y, vec3.z, 1.0f);
		return r;
	}
};

class Light :public Component
{
public:
	Light::Light(GameObject* g, Transform* t) :Component(g, t)
	{
		name = (char*)typeid(*this).name();
	}
	Light::~Light();
	void Awake()override;
	//ライトの追加
	void AddLight(DirectionalLight* l);
	//ベクター取得
	const vector<DirectionalLight*> GetLight();
	//ライトの数取得
	int GetNum();
private:
	//ライトのポインタ達
	vector<DirectionalLight*> lightVec;
};