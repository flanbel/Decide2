#pragma once
#include "Collisiont2D.h"

//lŠpŒ`‚Ì‚ ‚½‚è”»’è
class SquareCollision :public Collison2D
{
public:
	SquareCollision(GameObject* g, Transform* t) : Collison2D(g, t)
	{
		name = (char*)typeid(*this).name();
	}
	bool Judgment(Vector2 point);
	bool Judgment(SquareCollision* Square);
private:
	//—§‚Ä•A‰¡•
	Vector2 size;
};