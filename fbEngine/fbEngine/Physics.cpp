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

//衝突したときに呼ばれる関数オブジェクト(地面用)
struct SweepResultGround : public btCollisionWorld::ConvexResultCallback
{
	bool isHit = false;									//衝突フラグ。
	Vector3 hitPos = Vector3(0.0f, -FLT_MAX, 0.0f);	//衝突点。
	Vector3 startPos = Vector3::zero;					//レイの始点。
	Vector3 hitNormal = Vector3::zero;				//衝突点の法線。
	btCollisionObject* me = nullptr;					//自分自身。自分自身との衝突を除外するためのメンバ。
	float dist = FLT_MAX;								//衝突点までの距離。一番近い衝突点を求めるため。FLT_MAXは単精度の浮動小数点が取りうる最大の値。

														//衝突したときに呼ばれるコールバック関数。
	virtual	btScalar	addSingleResult(btCollisionWorld::LocalConvexResult& convexResult, bool normalInWorldSpace)
	{
		if (convexResult.m_hitCollisionObject == me
			|| convexResult.m_hitCollisionObject->getUserIndex() == 5
			) {
			//自分に衝突した。or キャラクタ属性のコリジョンと衝突した。
			return 0.0f;
		}
		//衝突点の法線を引っ張ってくる。
		Vector3 hitNormalTmp = *(Vector3*)&convexResult.m_hitNormalLocal;
		//上方向と法線のなす角度を求める。
		float angle = hitNormalTmp.Dot(Vector3::up);
		angle = fabsf(acosf(angle));
		if (angle < D3DX_PI * 0.2f		//地面の傾斜が54度より小さいので地面とみなす。
			|| convexResult.m_hitCollisionObject->getUserIndex() == 999 //もしくはコリジョン属性が地面と指定されている。
			) {
			//衝突している。
			isHit = true;

			//ここから先要らない？

			Vector3 hitPosTmp = *(Vector3*)&convexResult.m_hitPointLocal;
			//衝突点の距離を求める。。
			Vector3 vDist;
			vDist.Subtract(hitPosTmp, startPos);
			float distTmp = vDist.Length();
			if (dist > distTmp) {
				//この衝突点の方が近いので、最近傍の衝突点を更新する。
				hitPos = hitPosTmp;
				hitNormal = *(Vector3*)&convexResult.m_hitNormalLocal;
				dist = distTmp;
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
	//
	dynamicWorld->setGravity(btVector3(0, -980.8, 0));
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
	//コリジョンのタイプを確認
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

const bool PhysicsWorld::FindOverlappedStage(btCollisionObject * colliObject, Vector3 s, Vector3 e) const
{
	SweepResultGround callback;
	btTransform start, end;
	Vector3 endP;
	start.setIdentity();
	end.setIdentity();
	start.setOrigin(btVector3(s.x, s.y, s.x));
	end.setOrigin(btVector3(e.x, e.y, e.z));

	callback.me = colliObject;
	callback.startPos.Set(s);
	dynamicWorld->convexSweepTest((const btConvexShape*)colliObject->getCollisionShape(), start, end, callback);
	return callback.isHit;
}

PhysicsWorld* PhysicsWorld::Instance()
{
	if(instance == nullptr)
	{
		instance = new PhysicsWorld();
	}
	return instance;
}