#pragma once
#include "fbEngine/GameObject.h"
//ƒQ[ƒ€ƒJƒƒ‰
class GameCamera :public GameObject
{
public:
	GameCamera(char* name) :GameObject(name) {}
	void Awake()override;
	void Update()override;
};