#include "CollisionObject.h"
#include "BoxCollider.h"
#include "GostCollision.h"

void CollisionObject::Initialize(int ID, float Life, Vector3 Size)
{
	transform->localScale = Vector3(Size.x, Size.y, Size.z);
	shape->Create(Vector3(Size.x, Size.y, Size.z));
	coll->Create(shape,ID);
}

void CollisionObject::Awake()
{
	shape = AddComponent<BoxCollider>();
	coll = AddComponent<GostCollision>();
}

void CollisionObject::Update()
{
	//とりあえずコンポーネントが無効なら
	if (!coll->enable)
	{
		GameObjectManager::AddRemoveList(this);
	}
}