#pragma once
#include "fbEngine/GameObject.h"
class SkinModel;
class Animation;
//オフスクリーンレンダリング
class ShowModel:public GameObject
{
public:
	ShowModel(char* name) :GameObject(name) {}
	~ShowModel();
	void Awake()override;
	void Update()override;
	void SetModel(SkinModel* m)
	{
		model = m;
	}
	void SetAnim(Animation* a)
	{
		anim = a;
	}
	SkinModel* GetModel()
	{
		return model;
	}
	Animation* GetAnim()
	{
		return anim;
	}
private:
	SkinModel* model;
	Animation* anim;
};