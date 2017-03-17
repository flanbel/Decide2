#include "Item.h"
#include "fbEngine/Skinmodel.h"
#include "fbEngine/SkinmodelData.h"
#include "fbEngine/Animation.h"
#include "fbEngine/RigidBody.h"
#include "fbEngine/BoxCollider.h"

void Item::Awake()
{
	SkinModel* model = AddComponent<SkinModel>();
	_Anim = AddComponent<Animation>();

	_Rigid = AddComponent<RigidBody>();
	BoxCollider* coll = AddComponent<BoxCollider>();

	coll->Create(Vector3(15, 50, 15));
	//ID:6
	_Rigid->Create(1, coll, 6,Vector3::zero, Vector3(0, 25, 0));

	SkinModelData* modeldata = new SkinModelData();
	modeldata->CloneModelData(SkinModelManager::LoadModel("Item/StandardSowrd.X"), _Anim);
	model->SetModelData(modeldata);

	//アイテムの持ち手のフレーム取得
	D3DXFRAME_DERIVED* frame = (D3DXFRAME_DERIVED*)D3DXFrameFind(modeldata->GetFrameRoot(), "Handle_Bone");
	if (frame)
	{
		_ItemHandleMat = &frame->TransformationMatrix;
	}

	_LifeTime = 10.0f;
	_Timer = 0.0f;
	_IsHave = false;
}

void Item::LateUpdate()
{
	if (_IsHave)
	{
		D3DXMATRIX world;
		D3DXMatrixMultiply(&world, _HandMat, _ItemHandleMat);
		transform->SetWorldMatrix(world);
	}
	//所有されてない
	else
	{
		_Timer += Time::DeltaTime();
		if (_Timer >= _LifeTime)
		{
			//自身を削除
			GameObjectManager::AddRemoveList(this);
		}
	}
}

bool Item::ToHave(const D3DXMATRIX* handmat)
{
	//持たれていない
	if (!_IsHave)
	{
		_HandMat = handmat;
		if (_ItemHandleMat && _HandMat)
		{
			_IsHave = true;
			return true;
		}
	}
	return false;
}

void Item::ToSeparate(const D3DXMATRIX& world)
{
	Vector3 pos;
	pos.x = world.m[3][0];
	pos.y = world.m[3][1];
	pos.z = world.m[3][2];
	transform->localPosition = pos;

	_HandMat = nullptr;
	_IsHave = false;
}