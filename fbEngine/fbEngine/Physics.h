#pragma once

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
	void AddCollision(Collision* coll);
	void RemoveCollision(Collision* coll);
	const Collision* FindOverlappedDamageCollision(btCollisionObject * colliObject,int id) const;
	const bool FindOverlappedStage(btCollisionObject * colliObject,Vector3 start,Vector3 end) const;

	static PhysicsWorld* Instance();
private:
	std::unique_ptr<btDefaultCollisionConfiguration>		collisionConfig;
	std::unique_ptr<btCollisionDispatcher>					collisionDispatcher;	//!<衝突解決処理。
	std::unique_ptr<btBroadphaseInterface>					overlappingPairCache;	//!<ブロードフェーズ。衝突判定の枝切り。
	std::unique_ptr<btSequentialImpulseConstraintSolver>	constraintSolver;		//!<コンストレイントソルバー。拘束条件の解決処理。
	std::unique_ptr<btDiscreteDynamicsWorld>				dynamicWorld;			//!<ワールド。

	static PhysicsWorld* instance;
};