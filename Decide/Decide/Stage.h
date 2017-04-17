#pragma once
#include "fbEngine/_Object/_GameObject/GameObject.h"

class Stage:public GameObject
{
public:
	Stage(const char* name) :GameObject(name) {}
	void Awake()override;
};