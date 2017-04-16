#include "AttackCollision.h"

void AttackCollision::Awake()
{
	_Collision = AddComponent<DamageCollision>();
}

void AttackCollision::Create(Collider * shape, const int & id, float & life, DamageCollision::DamageCollisonInfo & info)
{
	_Collision->Create(life, id, shape, info);
}
