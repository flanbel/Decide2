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

	D3DXMATRIX View(){ return view; }
	D3DXMATRIX Projection(){ return projection; }

	//画角設定
	void ViewAngle(float theta)
	{
		viewAngle = theta;
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
		aspect = asp;
	}

protected:
	//ビュー行列
	D3DXMATRIX view;
	//プロジェクション行列
	D3DXMATRIX projection;

	float viewAngle;	//画角
	float _near;		//近平面
	float _far;		//遠平面
	float aspect;		//アスペクト比
};