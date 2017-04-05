#include "stdafx.h"
#include "SquareCollision.h"

const bool SquareCollision::Judgment(const Vector2& point)
{
	Vector2 pos = Vector2(transform->GetPosition().x, transform->GetPosition().y);
	RECT rect = { pos.x - _Size.x / 2,pos.y + _Size.y / 2,pos.x + _Size.x / 2,pos.y - _Size.y / 2 };
	
	return (rect.top > point.y || rect.bottom < point.y ||
		rect.left > point.x || rect.right < point.x);
}

const bool SquareCollision::Judgment(const SquareCollision * Square)
{
	return false;
}
