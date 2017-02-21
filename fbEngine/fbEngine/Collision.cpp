#include "Collision.h"
#include "Collider.h"


Collision::~Collision()
{
	shape = nullptr;
	collisionObject = nullptr;
}

void Collision::Create(btCollisionObject * coll, Collider * s, int id)
{
	shape = s;
	isHit = false;
	//コリジョンオブジェクト設定
	collisionObject.reset(coll);
	collisionObject->setCollisionShape(shape->GetBody());
	collisionObject->setUserPointer(this);
	collisionObject->setUserIndex(id);

	//トランスフォーム設定
	btTransform worldTrans;
	worldTrans.setIdentity();
	//ポジション
	worldTrans.setOrigin(btVector3(transform->position.x, transform->position.y, transform->position.z));
	//回転
	worldTrans.setRotation(btQuaternion(transform->rotation.x, transform->rotation.y, transform->rotation.z, transform->rotation.w));

	collisionObject->setWorldTransform(worldTrans);
}

#ifdef _DEBUG
#include "GameObject.h"
#include "SkinModel.h"
#include "SkinModelData.h"
#endif // _DEBUG
void Collision::Awake()
{
#ifdef _DEBUG
	////四角いモデル描画
	//SkinModel* model = gameObject->AddComponent<SkinModel>();
	//SkinModelData* data = new SkinModelData();
	//data->CloneModelData(SkinModelManager::LoadModel("BoxCollision.X"));
	//model->SetModelData(data);
	//model->blendcolor = Color(1.0f, 0, 0, 0.7f);
#endif // _DEBUG
}