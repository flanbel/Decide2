#pragma once
#include "Component.h"
//2D‚ÌƒRƒŠƒWƒ‡ƒ“
class Collison2D:public Component
{
public:
	Collison2D(GameObject* g, Transform* t) : Component(g, t, typeid(this).name())
	{
		
	}
	Collison2D(GameObject* g, Transform* t,const char* classname) : Component(g, t,classname)
	{

	}
	virtual ~Collison2D() {};
};