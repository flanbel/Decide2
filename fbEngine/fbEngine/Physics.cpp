#include "Physics.h"
#include "RigidBody.h"
#include "Collision.h"

PhysicsWorld* PhysicsWorld::_Instance;

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
	//
	dynamicWorld->setGravity(btVector3(0, -980.8, 0));
}
void PhysicsWorld::Update()
{
	dynamicWorld->stepSimulation((btScalar)Time::DeltaTime());
	dynamicWorld->updateAabbs();
}
void PhysicsWorld::Render()
{

}
void PhysicsWorld::AddRigidBody(RigidBody* rb)
{
	dynamicWorld->addRigidBody((btRigidBody*)rb->GetCollisonObj());
}
void PhysicsWorld::RemoveRigidBody(RigidBody* rb)
{
	dynamicWorld->removeRigidBody((btRigidBody*)rb->GetCollisonObj());
}

void PhysicsWorld::AddCollision(Collision * coll)
{
	//コリジョンのタイプを確認
	int type = coll->GetCollisonObj()->getInternalType();
	dynamicWorld->addCollisionObject(coll->GetCollisonObj());
}

void PhysicsWorld::RemoveCollision(Collision * coll)
{
	dynamicWorld->removeCollisionObject(coll->GetCollisonObj());
}

const Collision * PhysicsWorld::FindOverlappedDamageCollision(btCollisionObject * colliObject,const int& id) const
{
	MyContactResultCallback callback;
	callback.queryCollisionObject = colliObject;
	callback.id = id;
	dynamicWorld->contactTest(colliObject, callback);
	

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

PhysicsWorld* PhysicsWorld::Instance()
{
	if(_Instance == nullptr)
	{
		_Instance = new PhysicsWorld();
	}
	return _Instance;
}