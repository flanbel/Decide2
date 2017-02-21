#pragma once
#include "Collision.h"
class Collider;
//‚ ‚½‚è”»’è‚Ì‚Ý‚ÌƒRƒŠƒWƒ‡ƒ“
class GostCollision :public Collision
{
public:
	GostCollision(GameObject* g, Transform* t) :Collision(g, t)
	{
		name = (char*)typeid(*this).name();
	};
	virtual ~GostCollision();
	void Create(Collider* shape, int id);
	void Update()override;
};