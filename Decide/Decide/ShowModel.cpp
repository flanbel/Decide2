#include "ShowModel.h"

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
