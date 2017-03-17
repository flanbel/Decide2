#pragma once
#include "fbEngine/GameObject.h"
class Sky :public GameObject
{
public:
	Sky(const char* name) :GameObject(name) {}
	void Awake()override;
	void Update()override;
};