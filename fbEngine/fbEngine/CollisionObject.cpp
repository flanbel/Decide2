#include "CollisionObject.h"
#include "BoxCollider.h"
#include "GostCollision.h"

void CollisionObject::Initialize(int ID, float Life, Vector3 size)
{
	transform->SetLocalScale(Vector3(size.x, size.y, size.z));
	_Shape->Create(Vector3(size.x, size.y, size.z));
	_Coll->Create(_Shape,ID);
}

void CollisionObject::Awake()
{
	_Shape = AddComponent<BoxCollider>();
	_Coll = AddComponent<GostCollision>();
}

void CollisionObject::Update()
{
	
}