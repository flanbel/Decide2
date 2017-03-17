#include "ModelOject.h"
#include "SkinModel.h"
#include "SkinModelData.h"
#include "Animation.h"
void ModelObject::Awake()
{
	_Model = AddComponent<SkinModel>();
	_Anim = AddComponent<Animation>();
}

void ModelObject::Update()
{

}

void ModelObject::LoadModel(char * path)
{
	SkinModelData* modeldata = new SkinModelData();
	modeldata->CloneModelData(SkinModelManager::LoadModel(path), _Anim);
	_Model->SetModelData(modeldata);
}
