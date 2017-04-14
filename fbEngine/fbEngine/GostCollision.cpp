#include "GostCollision.h"
#include "Collider.h"

GostCollision::~GostCollision()
{
	if (_CollisionObject)
	{
		//登録されているので削除
		PhysicsWorld::Instance()->RemoveCollision(this);
	}
}

void GostCollision::Create(Collider * shape, int id)
{
	//コリジョン生成
	Collision::Create(new btGhostObject, shape, id);
	//作業用変数
	//コリジョンオブジェクト
	btCollisionObject* coll = this->_CollisionObject.get();
	//フラグ
	int flags = coll->getCollisionFlags();
	//コリジョンに衝突しても物理的挙動をしない(干渉されない。)
	//ここでトリガー指定しないと物理的に干渉してくる、ゴーストなのに。
	coll->setCollisionFlags(flags | btCollisionObject::CollisionFlags::CF_NO_CONTACT_RESPONSE);
	//自身を登録
	PhysicsWorld::Instance()->AddCollision(this);
}

void GostCollision::Update()
{
	Collision::Update();
}