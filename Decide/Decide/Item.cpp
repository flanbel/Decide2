#include "Item.h"
#include "fbEngine/Skinmodel.h"
#include "fbEngine/SkinmodelData.h"
#include "fbEngine/Animation.h"
#include "fbEngine/RigidBody.h"
#include "fbEngine/BoxCollider.h"

void Item::Awake()
{
	SkinModel* model = AddComponent<SkinModel>();
	anim = AddComponent<Animation>();

	rigid = AddComponent<RigidBody>();
	BoxCollider* coll = AddComponent<BoxCollider>();

	coll->Create(Vector3(15, 50, 15));
	//ID:6
	rigid->Create(1, coll, 6,Vector3::zero, Vector3(0, 25, 0));

	SkinModelData* modeldata = new SkinModelData();
	modeldata->CloneModelData(SkinModelManager::LoadModel("Item/StandardSowrd.X"), anim);
	model->SetModelData(modeldata);

	//持ち手のフレーム取得
	D3DXFRAME_DERIVED* frame = (D3DXFRAME_DERIVED*)D3DXFrameFind(modeldata->GetFrameRoot(), "Handle");
	if (frame)
	{
		itemHandleMat = &frame->TransformationMatrix;
	}

	lifetime = 10.0f;
	timer = 0.0f;
	isPossessed = false;
}

void Item::LateUpdate()
{
	if (isPossessed)
	{
		D3DXMATRIX world;
		D3DXMatrixMultiply(&world, handMat, itemHandleMat);
		world.m[3][0] = handMat->m[3][0] - itemHandleMat->m[3][0];
		world.m[3][1] = handMat->m[3][1] - itemHandleMat->m[3][1];
		world.m[3][2] = handMat->m[3][2] - itemHandleMat->m[3][2];
		transform->WorldMatrix(world);
	}
	//所有されてない
	else
	{
		timer += Time::DeltaTime();
		if (timer >= lifetime)
		{
			//自身を削除
			GameObjectManager::AddRemoveList(this);
		}
	}
}

void Item::ToHave(D3DXMATRIX* m)
{
	handMat = m;
	if (itemHandleMat && handMat)
	{
		isPossessed = true;
	}
}

void Item::ToSeparate()
{
	isPossessed = false;
}
