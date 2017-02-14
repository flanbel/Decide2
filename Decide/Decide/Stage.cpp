#include "Stage.h"
#include "fbEngine/Skinmodel.h"
#include "fbEngine/SkinmodelData.h"
#include "fbEngine/RigidBody.h"
#include "fbEngine/MeshCollider.h"

#include "fbEngine/CollisionObject.h"

void Stage::Awake()
{
	SkinModel* model = AddComponent<SkinModel>();
	SkinModelData* modeldata = new SkinModelData();
	modeldata->CloneModelData(SkinModelManager::LoadModel("Island.X"));
	model->SetModelData(modeldata);
	model->SetSpecular(false);
	//model->SetReceive(false);

	RigidBody* rigid = AddComponent<RigidBody>();
	MeshCollider* mesh = AddComponent<MeshCollider>();

	mesh->Create(model);
	rigid->Create(0, mesh,7);

	//transform->localAngle.x = -90;
}
#include "Player.h"
void Stage::Update()
{
	
}
