#pragma once
#include "fbEngine/_Object/_GameObject/GameObject.h"
//セレクトシーンのカメラ
class SelectCamera :public GameObject
{
public:
	SelectCamera(const char* name) :GameObject(name) {}
	void Awake()override;
	void Update()override;
};