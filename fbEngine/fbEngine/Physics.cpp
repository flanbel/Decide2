#include "Physics.h"
#include "RigidBody.h"
#include "Collision.h"

PhysicsWorld* PhysicsWorld::_Instance = nullptr;

PhysicsWorld::PhysicsWorld()
{
	collisionConfig = NULL;
	collisionDispatcher = NULL;
	overlappingPairCache = NULL;
	constraintSolver = NULL;
	dynamicWorld = NULL;
}


PhysicsWorld::~PhysicsWorld()
{
	//権利破棄
	dynamicWorld.release();
	constraintSolver.release();
	overlappingPairCache.release();
	collisionDispatcher.release();
	collisionConfig.release();
}

void PhysicsWorld::Start()
{
	//物理エンジンを初期化。
	///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
	collisionConfig.reset(new btDefaultCollisionConfiguration());

	///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	collisionDispatcher.reset(new btCollisionDispatcher(collisionConfig.get()));

	///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
	overlappingPairCache.reset(new btDbvtBroadphase());

	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	constraintSolver.reset(new btSequentialImpulseConstraintSolver);

	dynamicWorld.reset(new btDiscreteDynamicsWorld(
		collisionDispatcher.get(),
		overlappingPairCache.get(),
		constraintSolver.get(),
		collisionConfig.get()
		));
	//物理ワールドの重力設定
	dynamicWorld->setGravity(btVector3(0.0f, -9.8f*100, 0.0f));
	//ゴーストコリジョンがペアを見つけるときに使うコールバック設定。
	//これを設定しないとゴーストオブジェクトは重なったペアを見つけることはできない。
	dynamicWorld->getPairCache()->setInternalGhostPairCallback(new MyGhostPairCallback);

}
void PhysicsWorld::Update()
{
	dynamicWorld->stepSimulation((btScalar)Time::DeltaTime());
	dynamicWorld->updateAabbs();
}

void PhysicsWorld::AddRigidBody(RigidBody * rb, short group, short mask)
{
	dynamicWorld->addRigidBody((btRigidBody*)rb->GetCollisonObj(),group,mask);
}

void PhysicsWorld::RemoveRigidBody(RigidBody* rb)
{
	dynamicWorld->removeRigidBody((btRigidBody*)rb->GetCollisonObj());
}

void PhysicsWorld::AddCollision(Collision* coll, short group, short mask)
{
	dynamicWorld->addCollisionObject(coll->GetCollisonObj(),group,mask);
}

void PhysicsWorld::RemoveCollision(Collision * coll)
{
	dynamicWorld->removeCollisionObject(coll->GetCollisonObj());
}

const Vector3 PhysicsWorld::ClosestRayTest(const Vector3& f, const Vector3& t)
{
	//始点と終点を設定
	btVector3 from(f.x, f.y, f.z), to(t.x, t.y, t.z);
	//最も近かったを返すコールバック作成
	btCollisionWorld::ClosestRayResultCallback call(from, to);
	call.m_hitPointWorld = btVector3(0, 0, 0);
	//レイを飛ばす
	dynamicWorld->rayTest(from, to, call);
	return Vector3(call.m_hitPointWorld.x(), call.m_hitPointWorld.y(), call.m_hitPointWorld.z());
}

const Collision * PhysicsWorld::FindHitCollision(Collision * coll,const int& id) const
{
	MyContactResultCallback callback;
	callback.queryCollisionObject = coll;
	callback.id = id;
	dynamicWorld->contactTest(coll->GetCollisonObj(), callback);

	return callback.hitObject;
}

const Collision * PhysicsWorld::SearchCollisionByName(Collision * coll, const char * name, const int & id) const
{
	FindNameContactResultCallback callback;
	callback.queryCollisionObject = coll;
	callback.name = name;
	callback.id = id;
	dynamicWorld->contactTest(coll->GetCollisonObj(), callback);

	return callback.hitObject;
}

const SweepResultGround PhysicsWorld::FindOverlappedStage(btCollisionObject * colliObject,const Vector3& s,const Vector3& e) const
{
	SweepResultGround callback;
	btTransform start, end;
	//初期化
	start.setIdentity();
	end.setIdentity();
	//ポジション設定
	start.setOrigin(btVector3(s.x, s.y, s.z));
	end.setOrigin(btVector3(e.x, e.y, e.z));

	callback.me = colliObject;
	callback.startPos.Set(s);
	dynamicWorld->convexSweepTest((const btConvexShape*)colliObject->getCollisionShape(), start, end, callback);
	return callback;
}