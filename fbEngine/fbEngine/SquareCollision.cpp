#include "stdafx.h"
#include "SquareCollision.h"

bool SquareCollision::Judgment(Vector2 point)
{
	Vector2 pos = Vector2(transform->position.x, transform->position.y);
	RECT rect = { pos.x - size.x / 2,pos.y + size.y / 2,pos.x + size.x / 2,pos.y - size.y / 2 };
	
	return (rect.top > point.y || rect.bottom < point.y ||
		rect.left > point.x || rect.right < point.x);
}

bool SquareCollision::Judgment(SquareCollision * Square)
{
	return false;
}
