#include "Stage.h"

#include "fbEngine\_Object\_Component\_3D\SkinModel.h"
#include "fbEngine\_Object\_Component\_Physics\RigidBody.h"

void Stage::Awake()
{
	SkinModel* model = AddComponent<SkinModel>();
	SkinModelData* modeldata = new SkinModelData();
	modeldata->CloneModelData(SkinModelManager::LoadModel("Island.X"));
	model->SetModelData(modeldata);
	model->SetModelEffect(ModelEffectE::SPECULAR,false);

	RigidBody* rigid = AddComponent<RigidBody>();
	MeshCollider* mesh = AddComponent<MeshCollider>();
	//メッシュコライダー生成
	mesh->Create(model);
	rigid->SetFilter((short)Collision_Filter::STAGE,(short)fbCollisionFilterE::ALLFILTER - (short)Collision_Filter::DAMAGE);
	rigid->Create(0, mesh,Collision_Attribute::STAGE);
}