#pragma once
#include "fbEngine/GameObject.h"
class Stage:public GameObject
{
public:
	Stage(char* name) :GameObject(name) {}
	void Awake()override;
	void Update()override;
};