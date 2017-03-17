#pragma once
#include "fbEngine/GameObject.h"
//ƒQ[ƒ€ƒ‰ƒCƒg
class GameLight :public GameObject
{
public:
	GameLight(const char* name) :GameObject(name) {}
	void Awake()override;
	void Update()override;
};