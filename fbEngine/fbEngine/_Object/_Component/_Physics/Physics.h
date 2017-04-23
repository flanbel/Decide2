#pragma once
#include "_Include\PhysicsCallback.h"

class PhysicsWorld
{
public:
	PhysicsWorld();
	~PhysicsWorld();
	void Start();
	void Update();

	//ワールドに剛体追加
	void AddRigidBody(RigidBody* rb, short group = (short)fbCollisionFilterE::A, short mask = (short)fbCollisionFilterE::ALLFILTER);
	//ワールドから剛体削除
	void RemoveRigidBody(RigidBody* rb);
	//ワールドにコリジョン追加
	void AddCollision(Collision* coll, short group = (short)fbCollisionFilterE::A, short mask = (short)fbCollisionFilterE::ALLFILTER);
	//ワールドからコリジョン削除
	void RemoveCollision(Collision* coll);

	//ヒットした中で最も近いコリジョンを取得
	//Collison*　コリジョン
	//int　指定したコリジョンの属性とのみ当たりをとる
	const Collision* ClosestContactTest(Collision * coll,const int& attr = (const int)fbCollisionAttributeE::ALL) const;
	//ヒットしたコリジョンを全て取得
	//Collison*　コリジョン
	//int　指定したコリジョンの属性とのみ当たりをとる
	vector<Collision*> AllHitsContactTest(Collision * coll, const int& attr = (const int)fbCollisionAttributeE::ALL) const;
	//レイを飛ばしてヒットした中で最も近かったものを取得
	//Vector3　レイの始点
	//Vector3　レイの終点
	//int　指定したコリジョンの属性とのみ当たりをとる
	const Vector3 ClosestRayTest(const Vector3& from, const Vector3& to, const int& attr = (const int)fbCollisionAttributeE::ALL);
	//受け取ったコリジョンを始点から終点まで飛ばしてヒットした中で最も近かったものを取得
	//Collison* 飛ばすコリジョン
	//Vector3　始点
	//Vector3　終点
	//int　指定したコリジョンの属性とのみ当たりをとる
	const Collision* ClosestConvexSweepTest(Collision * coll, const Vector3& start, const Vector3& end,const int& attr = (const int)fbCollisionAttributeE::ALL) const;
	//地面用当たり判定
	const fbPhysicsCallback::SweepResultGround FindOverlappedStage(btCollisionObject * colliObject,const Vector3& start,const Vector3& end) const;

	//ダイナミックワールド取得
	btDiscreteDynamicsWorld* GetDynamicWorld()
	{
		return dynamicWorld.get();
	}

	static PhysicsWorld* Instance()
	{
		if (_Instance == nullptr)
		{
			_Instance = new PhysicsWorld();
		}
		return _Instance;
	}
private:
	std::unique_ptr<btDefaultCollisionConfiguration>		collisionConfig;
	std::unique_ptr<btCollisionDispatcher>					collisionDispatcher;	//!<衝突解決処理。
	std::unique_ptr<btBroadphaseInterface>					overlappingPairCache;	//!<ブロードフェーズ。衝突判定の枝切り。
	std::unique_ptr<btSequentialImpulseConstraintSolver>	constraintSolver;		//!<コンストレイントソルバー。拘束条件の解決処理。
	std::unique_ptr<btDiscreteDynamicsWorld>				dynamicWorld;			//!<ワールド。

	static PhysicsWorld* _Instance;
};