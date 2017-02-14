#pragma once
#include "fbEngine/GameObject.h"
//セレクトシーンのカメラ
class SelectCamera :public GameObject
{
public:
	SelectCamera(char* name) :GameObject(name) {}
	void Awake()override;
	void Update()override;
};