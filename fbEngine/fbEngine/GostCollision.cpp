#include "GostCollision.h"
#include "Collider.h"

GostCollision::~GostCollision()
{
	if (collisionObject)
	{
		//登録されているので削除
		PhysicsWorld::Instance()->RemoveCollision(this);
	}
}

void GostCollision::Create(Collider * shape, int id)
{
	//生成
	Collision::Create(new btGhostObject, shape, id);
	//ここでトリガー指定しないと物理的に干渉してくる、ゴーストなのに。
	this->collisionObject.get()->setCollisionFlags(collisionObject.get()->getCollisionFlags() | btCollisionObject::CollisionFlags::CF_NO_CONTACT_RESPONSE);
	//自身を登録
	PhysicsWorld::Instance()->AddCollision(this);
}

void GostCollision::Update()
{
	//位置更新
	btTransform worldTrans;
	//トランスフォーム取得
	worldTrans = collisionObject->getWorldTransform();
	worldTrans.setOrigin(btVector3(transform->position.x, transform->position.y, transform->position.z));
}