#include "ShowModel.h"
#include "fbEngine/SkinModel.h"
#include "fbEngine/Animation.h"
ShowModel::~ShowModel()
{
	SAFE_DELETE(_Model);
}
void ShowModel::Awake()
{
	_Model = new SkinModel(this,transform);
	_Model->Awake();
}

void ShowModel::Update()
{
	
}
