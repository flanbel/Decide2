#include "Sky.h"

#include "fbEngine\_Object\_Component\_3D\SkinModel.h"

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
