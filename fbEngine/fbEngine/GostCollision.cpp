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
	//トランスフォーム取得
	btTransform& worldTrans = collisionObject->getWorldTransform();
	//位置更新
	worldTrans.setOrigin(btVector3(transform->position.x, transform->position.y, transform->position.z));
	//回転
	worldTrans.setRotation(btQuaternion(transform->rotation.x, transform->rotation.y, transform->rotation.z, transform->rotation.w));

}