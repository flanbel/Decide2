#include "stdafx.h"
#include "CircleCollision.h"

bool CircleCollision::Judgment(Vector2 point)
{
	//ベクトルを求める
	Vector2 pos = { transform->position.x,transform->position.y };
	Vector2 vec = point - pos;
	//長さが半径以下ならヒット
	return (vec.Length() <= radius);
}

bool CircleCollision::Judgment(CircleCollision * circle)
{
	return false;
}
