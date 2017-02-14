#include "Physics.h"
#include "RigidBody.h"
#include "Collision.h"


PhysicsWorld* PhysicsWorld::instance;

struct MyContactResultCallback : public btCollisionWorld::ContactResultCallback
{
	MyContactResultCallback()
	{

	}
	btCollisionObject* queryCollisionObject = nullptr;
	Collision* hitObject = nullptr;
	float distSq = FLT_MAX;
	int id = 0;
	//衝突された時のコールバック関数
	virtual	btScalar	addSingleResult(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1) override
	{
		const Vector3* vColl0Pos = (Vector3*)(&colObj0Wrap->getWorldTransform().getOrigin());
		const Vector3* vColl1Pos = (Vector3*)(&colObj1Wrap->getWorldTransform().getOrigin());
		Vector3 vDist;
		vDist.Subtract(*vColl0Pos, *vColl1Pos);
		float distTmpSq = vDist.LengthSq();
		Collision* hitObjectTmp;
		if (distTmpSq < distSq) {
			//こちらの方が近い。
			if (queryCollisionObject == colObj0Wrap->getCollisionObject()) {
				hitObjectTmp = (Collision*)colObj1Wrap->getCollisionObject()->getUserPointer();
			}
			else {
				hitObjectTmp = (Collision*)colObj0Wrap->getCollisionObject()->getUserPointer();
			}
			//リジッドボディ同士のあたり判定はまだ実装してない
			if(hitObjectTmp && id == hitObjectTmp->GetCollisonObj()->getUserIndex()){
				distSq = distTmpSq;
				hitObject = hitObjectTmp;
				hitObject->SetHit(true);
			}
		}

		return 0.0f;
	}
};

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

	dynamicWorld->setGravity(btVector3(0, -98.8, 0));
}
void PhysicsWorld::Update()
{
	dynamicWorld->stepSimulation(Time::DeltaTime());
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
	int type = coll->GetCollisonObj()->getInternalType();
	dynamicWorld->addCollisionObject(coll->GetCollisonObj());
}

void PhysicsWorld::RemoveCollision(Collision * coll)
{
	dynamicWorld->removeCollisionObject(coll->GetCollisonObj());
}

const Collision * PhysicsWorld::FindOverlappedDamageCollision(btCollisionObject * colliObject,int id) const
{
	MyContactResultCallback callback;
	callback.queryCollisionObject = colliObject;
	callback.id = id;
	dynamicWorld->contactTest(colliObject, callback);

	return callback.hitObject;
}

PhysicsWorld* PhysicsWorld::Instance()
{
	if(instance == nullptr)
	{
		instance = new PhysicsWorld();
	}
	return instance;
}