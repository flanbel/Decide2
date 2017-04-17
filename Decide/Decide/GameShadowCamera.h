#pragma once
#include "fbEngine/_Object\_GameObject/GameObject.h"
//ƒQ[ƒ€‚Åg‚¤‰e‚ğì‚é‚½‚ß‚ÌƒJƒƒ‰
class GameShadowCamera :public GameObject
{
public:
	GameShadowCamera(const char* name) :GameObject(name) {}
	void Awake()override;
	void Update()override;
};