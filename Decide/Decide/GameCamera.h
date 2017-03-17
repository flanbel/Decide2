#pragma once
#include "fbEngine/GameObject.h"
//ƒQ[ƒ€ƒJƒƒ‰
class GameCamera :public GameObject
{
public:
	GameCamera(const char* name) :GameObject(name) {}
	void Awake()override;
	void Update()override;
};