#include "Item.h"

#include "AttackCollision.h"

namespace
{
	//アイテムの情報
	ItemData itemList[] =
	{
		{
			"Item/bom.X",
			Vector3(30,35,30),
			Vector3(0,0,0)
		},
		{
			"Item/StandardSowrd.X",
			Vector3(15,80,15),
			Vector3(0,40,0)
		}
	};
}

Item::Item(const char * name) :
	GameObject(name),
	_ItemIdx(-1),
	_LifeTime(10.0f),
	_Timer(0.0f),
	_Move(Vector3::zero),
	_Attack(nullptr),
	_IsHave(false),
	_ItemHandleMat(nullptr),
	_HandMat(nullptr)
{

}

Item::~Item()
{
	if(_Attack)
	{
		INSTANCE(GameObjectManager)->AddRemoveList(_Attack);
	}
}

void Item::Awake()
{
	SkinModel* model = AddComponent<SkinModel>();
	_Anim = AddComponent<Animation>();

	BoxCollider* coll = AddComponent<BoxCollider>();
	_Rigid = AddComponent<RigidBody>();

	//アイテムの種類決定
	_ItemIdx = Random::Range(0, 1);
	coll->Create(itemList[_ItemIdx].collisionsize);
	//フィルター設定
	_Rigid->SetFilter((short)Collision_Filter::ITEM, (short)fbCollisionFilterE::ALLFILTER - (short)Collision_Filter::DAMAGE);
	//コリジョン生成
	_Rigid->Create(1, coll, Collision_ID::ITEM , Vector3::zero, itemList[_ItemIdx].offset);

	//モデルセット。
	SkinModelData* modeldata = new SkinModelData();
	modeldata->CloneModelData(SkinModelManager::LoadModel(itemList[_ItemIdx].modelname), _Anim);
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
	if (_IsHave)
	{
		D3DXMATRIX world;
		//掛ける順番が大切（子×親）
		D3DXMatrixMultiply(&world, _ItemHandleMat, _HandMat);
		//ワールド行列更新
		transform->SetLocalPosition(Vector3(world._41, world._42, world._43));
		transform->SetWorldMatrix(world);
	}
	//所有されてない &&
	//投げられていない
	else if(_Move.Length() > 0.0f)
	{
		_Timer += Time::DeltaTime();
		if (_Timer >= _LifeTime)
		{
			//自身を削除
			INSTANCE(GameObjectManager)->AddRemoveList(this);
		}
	}

	//移動量がある時
	if (_Move.Length() > 0.0f)
	{
		//移動前のポジション保持
		Vector3 bufpos = transform->GetLocalPosition();
		//移動
		transform->SetLocalPosition(bufpos + _Move);
		_Rigid->Update();
		//当たり判定
		const Collision *coll = (const Collision*)PhysicsWorld::Instance()->FindHitCollision(_Rigid, Collision_ID::STAGE);
		if (coll)
		{
			_Move = Vector3::zero;
			transform->SetLocalPosition(bufpos);
			//攻撃判定削除
			INSTANCE(GameObjectManager)->AddRemoveList(_Attack);
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
			//他のコリジョンと衝突しないようにフラグを設定。
			_Rigid->GetCollisonObj()->setCollisionFlags(btCollisionObject::CollisionFlags::CF_NO_CONTACT_RESPONSE);
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

		//攻撃判定生成
		_Attack = INSTANCE(GameObjectManager)->AddNew<AttackCollision>("Throw", 1);
		SphereCollider* sphere = _Attack->AddComponent<SphereCollider>();
		Vector3 size = itemList[_ItemIdx].collisionsize;
		size.Scale(1.1f);
		sphere->Create(size.x);
		_Attack->GetComponent<Transform>()->SetParent(this->transform);

		DamageCollision::DamageCollisonInfo info;
		//ダメージ
		info.Damage = Random::Range(6, 12);
		//吹き飛ばし力
		info.Blown = vec;
		info.Blown.Normalize();
		info.Blown.Scale(60.0f);
		//硬直時間
		info.Rigor = 0.2f;
		float life = -1.0f;
		_Attack->Create(sphere, idx, life, info);
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
	//変更したフラグを消す。
	_Rigid->GetCollisonObj()->setCollisionFlags(0);
}