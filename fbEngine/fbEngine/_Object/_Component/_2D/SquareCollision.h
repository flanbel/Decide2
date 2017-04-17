#pragma once
#include "Collisiont2D.h"

//四角形のあたり判定
class SquareCollision :public Collison2D
{
public:
	SquareCollision(GameObject* g, Transform* t) : Collison2D(g, t, typeid(this).name())
	{
		
	}
	//四角形と点の判定
	const bool Judgment(const Vector2& point);
	//四角形同士の判定(未実装)
	const bool Judgment(const SquareCollision* Square);
private:
	//立て幅、横幅
	Vector2 _Size;
};