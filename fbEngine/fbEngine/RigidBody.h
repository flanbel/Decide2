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
	void SetGravity(Vector3 set);
	void SetGravity(btVector3& set);
	void SetGravity(float x, float y, float z);
private:
	btDefaultMotionState*	myMotionState;	//モーションステート。
};

