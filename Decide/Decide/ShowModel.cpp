#include "ShowModel.h"
#include "fbEngine/SkinModel.h"
#include "fbEngine/Animation.h"
ShowModel::~ShowModel()
{
	SAFE_DELETE(model);
}
void ShowModel::Awake()
{
	model = new SkinModel(this,transform);
	model->Awake();
}

void ShowModel::Update()
{
	
}
