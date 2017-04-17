#include "RigidBody.h"

RigidBody::RigidBody(GameObject* g, Transform* t) :
	Collision(g,t,typeid(this).name()),
	myMotionState(NULL)
{
	
}

RigidBody::~RigidBody()
{
	Release();
}
void RigidBody::Awake()
{
	Collision::Awake();
}
void RigidBody::Release()
{
	if (_CollisionObject)
	{
		//登録されているので削除
		PhysicsWorld::Instance()->RemoveRigidBody(this);
		SAFE_DELETE(myMotionState);
	}
}

void RigidBody::Update()
{
	Collision::Update();
}

void RigidBody::LateUpdate()
{
	//シュミレート後の結果を送る
	btTransform trans = _CollisionObject->getWorldTransform();
	
	btVector3 pos = trans.getOrigin();
	btQuaternion rot = trans.getRotation();
	//ポジションはずらされているので戻す。
	transform->SetLocalPosition(Vector3(pos.x() - _Offset.x, pos.y() - _Offset.y, pos.z() - _Offset.z));
	//クォータニオンを各軸の回転量に変換
	//Transform->localAngle = Vector3(D3DXToDegree(asin(rot.x())*2.0f), D3DXToDegree(asin(rot.y())*2.0f), D3DXToDegree(asin(rot.z())*2.0f));
}

//void RigidBody::Create(RigidBodyInfo& rbInfo)
void RigidBody::Create(float mass, Collider* coll, int id, Vector3 inertia,Vector3 off)
{
	//前回の内容解放
	Release();
	//回転と移動設定
	btTransform StartTrans;
	StartTrans.setIdentity();
	StartTrans.setOrigin(btVector3(transform->GetPosition().x + _Offset.x, transform->GetPosition().y + _Offset.y, transform->GetPosition().z + _Offset.z));
	StartTrans.setRotation(btQuaternion(transform->GetRotation().x, transform->GetRotation().y, transform->GetRotation().z, transform->GetRotation().w));
	myMotionState = new btDefaultMotionState(StartTrans);
	//剛体を作成。
	btRigidBody::btRigidBodyConstructionInfo btRbInfo(mass, myMotionState, coll->GetBody(), btVector3(inertia.x, inertia.y, inertia.z));
	Collision::Create(new btRigidBody(btRbInfo), coll, id, off);
	//自身を登録
	PhysicsWorld::Instance()->AddRigidBody(this, _FilterGroup, _FilterMask);
}

void RigidBody::SetGravity(Vector3 set)
{
	//アップキャスト
	btRigidBody* rigit = (btRigidBody*)_CollisionObject.get();
	rigit->setGravity(btVector3(set.x, set.y, set.z));
}

void RigidBody::SetGravity(btVector3& set)
{
	//アップキャスト
	btRigidBody* rigit = (btRigidBody*)_CollisionObject.get();
	rigit->setGravity(set);
}

void RigidBody::SetGravity(float x, float y, float z)
{
	//アップキャスト
	btRigidBody* rigit = (btRigidBody*)_CollisionObject.get();
	rigit->setGravity(btVector3(x, y, z));
}
