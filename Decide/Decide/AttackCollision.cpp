#include "AttackCollision.h"
#include "fbEngine/BoxCollider.h"

void AttackCollision::Awake()
{
	shape = AddComponent<BoxCollider>();
	coll = AddComponent<DamageCollision>();
}

void AttackCollision::Update()
{
	
}

void AttackCollision::Create(const int & ID, float & Life, Vector3 & Size, DamageCollision::DamageCollisonInfo & info)
{
	shape->Create(Vector3(Size.x, Size.y, Size.z));
	transform->SetLocalScale(Size);
	coll->Create(Life, ID, shape, info);
}
