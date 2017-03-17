#pragma once
#include "fbEngine/GameObject.h"
class Stage:public GameObject
{
public:
	Stage(const char* name) :GameObject(name) {}
	void Awake()override;
	void Update()override;
};