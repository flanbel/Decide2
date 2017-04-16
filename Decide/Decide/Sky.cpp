#include "Sky.h"
#include "fbEngine/Skinmodel.h"
#include "fbEngine/SkinmodelData.h"
#include "fbEngine/RigidBody.h"
#include "fbEngine/MeshCollider.h"

#include "fbEngine/CollisionObject.h"

void Sky::Awake()
{
	SkinModel* model = AddComponent<SkinModel>();
	SkinModelData* modeldata = new SkinModelData();
	modeldata->CloneModelData(SkinModelManager::LoadModel("Sky.X"));
	model->SetModelData(modeldata);
	model->SetModelEffect(ModelEffectE::RECEIVE_SHADOW, false);
	model->SetModelEffect(ModelEffectE::CAST_SHADOW, false);
	model->SetSky(true);
	transform->SetLocalAngle(Vector3(15, 0, 0));
}
