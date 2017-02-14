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