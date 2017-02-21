#include "AttackCollision.h"
#include "fbEngine/BoxCollider.h"

void AttackCollision::Initialize(int ID, float Life, Vector3 Size, DamageCollision::DamageCollisonInfo info)
{
	shape->Create(Vector3(Size.x, Size.y, Size.z));
	transform->localScale = Size;
	coll->Create(Life, ID, shape, info);
}

void AttackCollision::Awake()
{
	shape = AddComponent<BoxCollider>();
	coll = AddComponent<DamageCollision>();
}

void AttackCollision::Update()
{
	if(coll->GetHit() || !coll->enable)
	{
		GameObjectManager::AddRemoveList(this);
	}
}
