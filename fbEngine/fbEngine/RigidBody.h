#pragma once
#include "Collision.h"
class Collider;

//剛体クラス。
class RigidBody:public Collision
{
public:
	RigidBody(GameObject* g, Transform* t);
	~RigidBody();
	void Update()override;
	void LateUpdate()override;
	void Release();
	//void Create(RigidBodyInfo& rbInfo);
	void Create(float mass,Collider* coll, int id, Vector3 inertia = Vector3::zero,Vector3 off = Vector3::zero);
	bool IsGround()
	{
		return isGround;
	}
	//地面についているかどうか
	bool isGround;
private:
	Vector3 offset;
	btDefaultMotionState*	myMotionState;	//モーションステート。
};

