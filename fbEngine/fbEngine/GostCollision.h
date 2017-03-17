#pragma once
#include "Collision.h"
class Collider;
//‚ ‚½‚è”»’è‚Ì‚Ý‚ÌƒRƒŠƒWƒ‡ƒ“
class GostCollision :public Collision
{
public:
	GostCollision(GameObject* g, Transform* t) :Collision(g, t, typeid(this).name())
	{
		
	};
	GostCollision(GameObject* g, Transform* t,const char* classname) :Collision(g, t, classname)
	{

	};
	virtual ~GostCollision();
	void Create(Collider* shape, int id);
	void Update()override;
};