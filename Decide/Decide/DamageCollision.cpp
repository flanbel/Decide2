#include "DamageCollision.h"

void DamageCollision::Create(float l, int id, Collider * shape, DamageCollisonInfo in)
{
	life = l;
	time = 0;
	info = in;
	GostCollision::Create(shape, id);
}

void DamageCollision::Update()
{
	//õ–½Šm”F
	time += (float)Time::DeltaTime();
	if (life > 0 && 
		time > life)
	{
		//€
		enable = false;
		return;
	}
}