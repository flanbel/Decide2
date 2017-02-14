#pragma once
#include "fbEngine/GameObject.h"

class Animation;

class Item:public GameObject
{
public:
	Item(char* name) :GameObject(name)
	{
		
	}
	void Awake()override;
	void LateUpdate()override;
	//アイテムを持つときに呼び出す関数
	void ToHave(D3DXMATRIX* m);
	//離す
	void ToSeparate();
private:
	Animation* anim;
	RigidBody* rigid;
	float lifetime;	//寿命
	float timer;
	bool isPossessed;//所持されている。
	//持ち手(handle)の行列(アイテム側)
	D3DXMATRIX* itemHandleMat;
	//持つボーンの行列(キャラクター側)
	D3DXMATRIX* handMat;
};