#pragma once
#include "fbEngine/_Object/_GameObject/GameObject.h"
class Sky :public GameObject
{
public:
	Sky(const char* name) :GameObject(name) {}
	void Awake()override;
};