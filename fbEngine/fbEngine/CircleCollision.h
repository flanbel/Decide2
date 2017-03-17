#pragma once
#include "Collisiont2D.h"

//‰~‚Ì‚ ‚½‚è”»’è
class CircleCollision :public Collison2D
{
public:
	CircleCollision(GameObject* g, Transform* t) : Collison2D(g, t, typeid(this).name())
	{
		
	}
	bool Judgment(Vector2 point);
	bool Judgment(CircleCollision* circle);
	void SetRadius(float r)
	{
		radius = r;
	}
private:
	float radius;
};