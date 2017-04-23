#include "AttackCollision.h"

void AttackCollision::Awake()
{
	_Collision = AddComponent<DamageCollision>();
}

void AttackCollision::Create(Collider * shape, const int& owner, const int & id, float & life, DamageCollision::DamageCollisonInfo & info)
{
	_Collision->Create(owner,life, id, shape, info);
}
