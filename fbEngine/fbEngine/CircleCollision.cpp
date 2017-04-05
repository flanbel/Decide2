#include "stdafx.h"
#include "CircleCollision.h"

const bool CircleCollision::Judgment(const Vector2& point)
{
	//ベクトルを求める
	Vector2 pos = { transform->GetPosition().x,transform->GetPosition().y };
	Vector2 vec = point - pos;
	//長さが半径以下ならヒット
	return (vec.Length() <= _Radius);
}

const bool CircleCollision::Judgment(const CircleCollision * circle)
{
	return false;
}
