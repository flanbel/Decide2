#pragma once
#include "fbEngine/GameObject.h"
//ƒQ[ƒ€‚Åg‚¤‰e‚ğì‚é‚½‚ß‚ÌƒJƒƒ‰
class GameShadowCamera :public GameObject
{
public:
	GameShadowCamera(char* name) :GameObject(name) {}
	void Awake()override;
	void Update()override;
};