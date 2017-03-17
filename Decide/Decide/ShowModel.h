#pragma once
#include "fbEngine/GameObject.h"
class SkinModel;
class Animation;
//オフスクリーンレンダリング
class ShowModel:public GameObject
{
public:
	ShowModel(const char* name) :GameObject(name) {}
	~ShowModel();
	void Awake()override;
	void Update()override;
	void SetModel(SkinModel* m)
	{
		_Model = m;
	}
	void SetAnim(Animation* a)
	{
		_Anim = a;
	}
	SkinModel* GetModel()
	{
		return _Model;
	}
	Animation* GetAnim()
	{
		return _Anim;
	}
private:
	SkinModel* _Model;
	Animation* _Anim;
};