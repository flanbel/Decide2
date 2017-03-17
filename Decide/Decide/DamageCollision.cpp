#include "DamageCollision.h"

void DamageCollision::Update()
{
	GostCollision::Update();
	//Žõ–½Šm”F
	_Timer += (float)Time::DeltaTime();
	if (_Life > 0.0f && 
		_Timer > _Life)
	{
		//Ž€
		enable = false;
		return;
	}
}

void DamageCollision::Create(const float& life, const int & id, Collider * shape, DamageCollisonInfo & in)
{
	_Life = life;
	_Timer = 0.0f;
	info = in;
	GostCollision::Create(shape, id);
}