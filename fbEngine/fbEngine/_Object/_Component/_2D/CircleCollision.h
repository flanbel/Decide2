#pragma once
#include "Collisiont2D.h"

//â~ÇÃÇ†ÇΩÇËîªíË
class CircleCollision :public Collison2D
{
public:
	CircleCollision(GameObject* g, Transform* t) : Collison2D(g, t, typeid(this).name())
	{
		
	}
	//â~Ç∆ì_ÇÃÇ†ÇΩÇËîªíË
	const bool Judgment(const Vector2& point);
	//ñ¢é¿ëï
	const bool Judgment(const CircleCollision* circle);
	//îºåaê›íË
	void SetRadius(float r)
	{
		_Radius = r;
	}
private:
	float _Radius;
};