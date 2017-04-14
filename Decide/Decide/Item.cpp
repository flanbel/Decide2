#include "Item.h"
#include "fbEngine/Skinmodel.h"
#include "fbEngine/SkinmodelData.h"
#include "fbEngine/Animation.h"
#include "fbEngine/RigidBody.h"
#include "fbEngine/BoxCollider.h"
#include "DamageCollision.h"
#include "AttackCollision.h"

namespace
{
	ItemData item[] = 
	{
		{
			"Item/bom.X",
			Vector3(30,30,30)
		},
		{
			"Item/StandardSowrd.X",
			Vector3(15,50,15)
		}
	};
}

Item::Item(const char * name) :
	GameObject(name),
	_LifeTime(10.0f),
	_Timer(0.0f),
	_Move(Vector3::zero),
	_IsHave(false),
	_ItemHandleMat(nullptr),
	_HandMat(nullptr)
{

}

void Item::Awake()
{
	SkinModel* model = AddComponent<SkinModel>();
	_Anim = AddComponent<Animation>();

	_Rigid = AddComponent<RigidBody>();
	BoxCollider* coll = AddComponent<BoxCollider>();

	int idx = Random::Range(0, 1);

	int height = item[idx].collisionsize.y;
	coll->Create(item[idx].collisionsize);
	//ID:6
	_Rigid->Create(1, coll, 6);

	SkinModelData* modeldata = new SkinModelData();
	modeldata->CloneModelData(SkinModelManager::LoadModel(item[idx].modelname), _Anim);
	model->SetModelData(modeldata);

	//アイテムの持ち手のフレーム取得
	D3DXFRAME_DERIVED* frame = (D3DXFRAME_DERIVED*)D3DXFrameFind(modeldata->GetFrameRoot(), "Handle_Bone");
	if (frame)
	{
		_ItemHandleMat = &frame->TransformationMatrix;
	}
}

void Item::Update()
{
	//移動量がある時
	if(_Move.Length() > 0.0f)
	{
		//移動前のポジション保持
		Vector3 bufpos = transform->GetLocalPosition();
		//移動
		transform->SetLocalPosition(bufpos + _Move);
		_Rigid->Update();
		//当たり判定
		const Collision *coll = (const Collision*)PhysicsWorld::Instance()->FindHitCollision(_Rigid, Collision_ID::STAGE);
		if(coll)
		{
			_Move = Vector3::zero;
			transform->SetLocalPosition(bufpos);
		}
	}
}

void Item::LateUpdate()
{
	if (_IsHave)
	{
		D3DXMATRIX world;
		//掛ける順番が大切（子×親）
		D3DXMatrixMultiply(&world, _ItemHandleMat, _HandMat);
		//ワールド行列更新
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
			_Move = Vector3::zero;
			return true;
		}
	}
	return false;
}

void Item::ToSeparate(const Vector3& vec, int idx)
{
	//投げる
	if (vec.Length() > 0)
	{
		//移動量設定
		_Move = vec;
		//ダメージコリジョン生成
		//あたり判定を出す
		AttackCollision* attack = GameObjectManager::AddNew<AttackCollision>("Throw", 1);
		Transform* t = attack->GetComponent<Transform>();
		//前に出す
		t->SetLocalPosition(transform->Local(Vector3(0, 60, 30)));
		t->SetLocalAngle(transform->GetLocalAngle());

		DamageCollision::DamageCollisonInfo info;
		//攻撃力補正
		info.Damage = 10;
		info.Blown = transform->Direction(Vector3(0.0f, 0.0f, 60.0f));
		//吹き飛ばし補正
		info.Rigor = 0.2f;
		float life = 0.2f;
		attack->Create(0, life, Vector3(50, 50, 50), info);

	}
	Vector3 pos;
	//手の位置に移動。
	pos.x = _HandMat->m[3][0];
	pos.y = _HandMat->m[3][1];
	pos.z = _HandMat->m[3][2];
	transform->SetLocalPosition(pos);
	//外す
	_HandMat = nullptr;
	_IsHave = false;
}