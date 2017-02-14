#include "ModelOject.h"
#include "SkinModel.h"
#include "SkinModelData.h"
#include "Animation.h"
void ModelObject::Awake()
{
	model = AddComponent<SkinModel>();
	anim = AddComponent<Animation>();
}

void ModelObject::Update()
{

}

void ModelObject::LoadModel(char * path)
{
	SkinModelData* modeldata = new SkinModelData();
	modeldata->CloneModelData(SkinModelManager::LoadModel(path), anim);
	model->SetModelData(modeldata);
}
