#include "GostCollision.h"

GostCollision::~GostCollision()
{
	if (_CollisionObject)
	{
		//物理ワールドから削除
		PhysicsWorld::Instance()->RemoveCollision(this);
	}
}

void GostCollision::Awake()
{
	Collision::Awake();
}

void GostCollision::Create(Collider * shape, int id)
{
	//コリジョン生成
	Collision::Create(new btGhostObject, shape, id);
	//生ポインタ取得してキャスト
	_GostObject = btGhostObject::upcast(this->_CollisionObject.get());
	//今設定されているフラグを取得
	int flags = _GostObject->getCollisionFlags();
	//他のコリジョンと衝突しないようにフラグを設定。
	_GostObject->setCollisionFlags(flags | btCollisionObject::CollisionFlags::CF_NO_CONTACT_RESPONSE);
	//物理ワールドに登録
	PhysicsWorld::Instance()->AddCollision(this,_FilterGroup,_FilterMask);
}

void GostCollision::Update()
{
	Collision::Update();
}

btAlignedObjectArray<btCollisionObject*> GostCollision::GetPairCollisions()
{
	//自身と重なっているコリジョン取得
	return _GostObject->getOverlappingPairs();
}
