#include "RigidBody.h"
#include "Collider.h"

RigidBody::RigidBody(GameObject* g, Transform* t) :
	Collision(g,t),
	myMotionState(NULL),
	offset(Vector3::zero)
{
	name = (char*)typeid(*this).name();
}


RigidBody::~RigidBody()
{
	Release();
}
void RigidBody::Release()
{
	if (collisionObject)
	{
		PhysicsWorld::Instance()->RemoveRigidBody(this);
		delete myMotionState;
		collisionObject = NULL;
		myMotionState = NULL;
	}
}

void RigidBody::Update()
{
	//ゲーム側での移動を送る
	btTransform& trans = collisionObject->getWorldTransform();
	trans.setOrigin(btVector3(transform->position.x + offset.x, transform->position.y + offset.y, transform->position.z + offset.z));
	trans.setRotation(btQuaternion(transform->rotation.x, transform->rotation.y, transform->rotation.z, transform->rotation.w));
}

void RigidBody::LateUpdate()
{
	//シュミレート後の結果を送る
	btTransform trans = collisionObject->getWorldTransform();
	
	btVector3 pos = trans.getOrigin();
	btQuaternion rot = trans.getRotation();
	transform->localPosition = Vector3(pos.x() - offset.x, pos.y() - offset.y, pos.z() - offset.z);
	//クォータニオンを各軸の回転量に変換
	//transform->localAngle = Vector3(D3DXToDegree(asin(rot.x())*2.0f), D3DXToDegree(asin(rot.y())*2.0f), D3DXToDegree(asin(rot.z())*2.0f));
}

//void RigidBody::Create(RigidBodyInfo& rbInfo)
void RigidBody::Create(float mass, Collider* coll, int id, Vector3 inertia,Vector3 off)
{
	offset = off;
	//前回の内容解放
	Release();
	//回転と移動設定
	btTransform StartTrans;
	StartTrans.setIdentity();
	StartTrans.setOrigin(btVector3(transform->position.x + offset.x, transform->position.y + offset.y, transform->position.z + offset.z));
	StartTrans.setRotation(btQuaternion(transform->rotation.x, transform->rotation.y, transform->rotation.z, transform->rotation.w));
	myMotionState = new btDefaultMotionState(StartTrans);
	//剛体を作成。
	btRigidBody::btRigidBodyConstructionInfo btRbInfo(mass, myMotionState, coll->GetBody(), btVector3(inertia.x, inertia.y, inertia.z));
	Collision::Create(new btRigidBody(btRbInfo), coll, id);
	//自身を登録
	PhysicsWorld::Instance()->AddRigidBody(this);
}

void RigidBody::SetGravity(Vector3 set)
{
	//アップキャスト
	btRigidBody* rigit = (btRigidBody*)collisionObject.get();
	rigit->setGravity(btVector3(set.x, set.y, set.z));
}

void RigidBody::SetGravity(btVector3& set)
{
	//アップキャスト
	btRigidBody* rigit = (btRigidBody*)collisionObject.get();
	rigit->setGravity(set);
}

void RigidBody::SetGravity(float x, float y, float z)
{
	//アップキャスト
	btRigidBody* rigit = (btRigidBody*)collisionObject.get();
	rigit->setGravity(btVector3(x, y, z));
}
