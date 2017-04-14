#pragma once
#include "PhysicsCallback.h"

class Collision;
class RigidBody;

class PhysicsWorld
{
public:
	PhysicsWorld();
	~PhysicsWorld();
	void Start();
	void Update();
	void Render();
	/*!
	* @brief	ダイナミックワールドを取得。
	*/
	btDiscreteDynamicsWorld* GetDynamicWorld()
	{
		return dynamicWorld.get();
	}
	void AddRigidBody(RigidBody* rb);
	void RemoveRigidBody(RigidBody* rb);
	void ConvexSweepTest(
		const btConvexShape* castShape,
		const btTransform& convexFromWorld,
		const btTransform& convexToWorld,
		btCollisionWorld::ConvexResultCallback& resultCallback,
		btScalar allowedCcdPenetration = 0.0f
		)
	{
		dynamicWorld->convexSweepTest(castShape, convexFromWorld, convexToWorld, resultCallback, allowedCcdPenetration);
	}
	//コリジョン追加
	void AddCollision(Collision* coll);
	//コリジョン
	void RemoveCollision(Collision* coll);

	//レイを飛ばして最も近かったものを
	const Vector3 ClosestRayTest(const Vector3& from,const Vector3& to);
	//当たっているコリジョン検索
	const Collision* FindHitCollision(Collision * coll,const int& id) const;
	//名前で検索する。
	const Collision* SearchCollisionByName(Collision * coll, const char* name, const int& id) const;
	//地面用当たり判定
	const SweepResultGround FindOverlappedStage(btCollisionObject * colliObject,const Vector3& start,const Vector3& end) const;

	static PhysicsWorld* Instance();
private:
	std::unique_ptr<btDefaultCollisionConfiguration>		collisionConfig;
	std::unique_ptr<btCollisionDispatcher>					collisionDispatcher;	//!<衝突解決処理。
	std::unique_ptr<btBroadphaseInterface>					overlappingPairCache;	//!<ブロードフェーズ。衝突判定の枝切り。
	std::unique_ptr<btSequentialImpulseConstraintSolver>	constraintSolver;		//!<コンストレイントソルバー。拘束条件の解決処理。
	std::unique_ptr<btDiscreteDynamicsWorld>				dynamicWorld;			//!<ワールド。

	static PhysicsWorld* _Instance;
};