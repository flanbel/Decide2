#pragma once
#include "Component.h" 

class Camera :public Component
{
public:
	Camera(GameObject* g, Transform* t) :
		Component(g, t, typeid(this).name())
	{
		
	}
	Camera(GameObject* g, Transform* t,const char* classname) :
		Component(g, t, classname)
	{

	}
	~Camera();
	void Awake();
	void Start();
	virtual void Update();

	void ViewMatrixUpdate();
	void ProjectionMatrixUpdate();

	//受け取ったワールドポジションをスクリーン座標に変換
	Vector2 WorldToScreen(Vector3 wpos);
	//受け取ったスクリーン座標をワールドポジションに変換(zは0になる。)
	Vector3 ScreenToWorld(Vector2 spos);

	const D3DXMATRIX& GetViewMat(){ return _View; }
	const D3DXMATRIX& GetProjectionMat(){ return _Projection; }

	//画角設定
	void SetViewAngle(const float& theta)
	{
		_ViewAngle = theta;
	}
	//近平面設定
	void SetNear(const float& n)
	{
		_near = n;
	}
	//遠平面設定
	void SetFar(const float& f)
	{
		_far = f;
	}
	//アスペクト比率設定
	void SetAspect(const float& asp)
	{
		_Aspect = asp;
	}
	//注視点設定
	void SetViewPoint(const Vector3& pos)
	{
		//アドレス確保されていない
		if (_ViewPoint == nullptr)
		{
			//確保するよ。
			_ViewPoint = new Vector3();
		}
		*_ViewPoint = pos;
	}
	//注視点のポインタを渡す。
	void SetViewPoint(Vector3* point)
	{
		if (_ViewPoint != nullptr)
		{
			//前のアドレス削除
			SAFE_DELETE(_ViewPoint);
		}
		_ViewPoint = point;
	}
	Vector3 GetViewPoint()
	{
		if (_ViewPoint == nullptr)
		{
			_ViewPoint = new Vector3();
		}
		return *_ViewPoint;
	}
protected:
	//ビュー行列
	D3DXMATRIX _View;
	//プロジェクション行列
	D3DXMATRIX _Projection;

	//注視点
	Vector3* _ViewPoint;
	float _ViewAngle;	//画角
	float _near;		//近平面
	float _far;		//遠平面
	float _Aspect;		//アスペクト比
};