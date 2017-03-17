#pragma once
#include "GameObject.h"

class SkinModel;
class Animation;
//3DModelを表示するゲームオブジェクト
class ModelObject :public GameObject
{
public:
	ModelObject(char* name) :GameObject(name) {}
	void Awake()override;
	void Update()override;
	void LoadModel(char* path);
private:
	SkinModel* _Model;
	Animation* _Anim;
};
