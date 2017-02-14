#pragma once
#include "Component.h"
//2D‚ÌƒRƒŠƒWƒ‡ƒ“
class Collison2D:public Component
{
public:
	Collison2D(GameObject* g, Transform* t) : Component(g, t)
	{
		name = (char*)typeid(*this).name();
	}
};