#pragma once
#include "Component.h" 

class Camera :public Component
{
public:
	Camera(GameObject* g, Transform* t) :Component(g, t, typeid(this).name())
	{
		
	}
	Camera(GameObject* g, Transform* t,const char* classname) :Component(g, t, classname)
	{

	}
	void Awake();
	void Start();
	virtual void Update();

	void ViewMatrixUpdate();
	void ProjectionMatrixUpdate();

	D3DXMATRIX View(){ return _View; }
	D3DXMATRIX Projection(){ return _Projection; }

	//画角設定
	void ViewAngle(float theta)
	{
		_ViewAngle = theta;
	}
	//近平面設定
	void Near(float n)
	{
		_near = n;
	}
	//遠平面設定
	void Far(float f)
	{
		_far = f;
	}
	//アスペクト比率設定
	void Aspect(float asp)
	{
		_Aspect = asp;
	}

protected:
	//ビュー行列
	D3DXMATRIX _View;
	//プロジェクション行列
	D3DXMATRIX _Projection;

	float _ViewAngle;	//画角
	float _near;		//近平面
	float _far;		//遠平面
	float _Aspect;		//アスペクト比
};