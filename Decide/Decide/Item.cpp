#include "Item.h"

#include "AttackCollision.h"
#include "Player.h"
#include "fbEngine\_Object\_GameObject\AnimationPlate.h"
#include "fbEngine\_Object\_GameObject\SoundSource.h"

//アイテムの最大数
static const int ITEM_MAX_NUM = 2;

namespace
{
	//アイテムの情報
	ItemData itemList[ITEM_MAX_NUM] =
	{
		{
			"Item/StandardSowrd.X",
			Vector3(15,80,15),
			Vector3(0,40,0),
			0,
			fbEngine::ItemTypeE::Slash
		},
		{
			"Item/bom.X",
			Vector3(30,35,30),
			Vector3(0,0,0),
			1,
			fbEngine::ItemTypeE::Throw
		}
	};
}

//無名空間だとフレンド関数にできなかったので名前を付けた。
namespace
{
	//各アイテムの投げられてヒットした時の動作を設定
	void Standard(Item* item, vector<Collision*> colls)
	{
		//あたったやつに小ダメージを与える。
		FOR(i, colls.size())
		{
			//属性を確認
			if ((colls[i]->GetCollisonObj()->getUserIndex() & Collision_Attribute::PLAYER) != 0)
			{
				Player* player = (Player*)colls[i]->gameObject;
				Vector3 blow = item->GetMove();
				blow.Normalize();
				blow.Scale(30.0f);
				player->Damage(item->GetOwner(), Random::Range(6, 14), blow, 0.5f);
			}
		}
	};

	void Bom(Item* item, vector<Collision*> colls)
	{
		//爆発アニメーション生成
		AnimationPlate *bomber = INSTANCE(GameObjectManager)->AddNew<AnimationPlate>("Bomber", 4);
		bomber->transform->SetPosition(item->transform->GetPosition());
		bomber->SetTexture(LOADTEXTURE("bomber.jpg"));
		bomber->SetSplit(6, 4, 6 * 4);
		bomber->SetSize(Vector2(256, 256));
		bomber->SetBlendMode(fbEngine::BlendModeE::Add);
		bomber->Play(1.0f, 1);
		bomber->SetBlendColor(Color::white * 1.8f);
		bomber->SetDelete(true);

		//爆発の効果音再生
		SoundSource* bomberSE = INSTANCE(GameObjectManager)->AddNew<SoundSource>("PunchSound", 0);
		bomberSE->Init("Asset/Sound/bomb3.wav");
		bomberSE->SetDelete(true);
		bomberSE->Play(false);

		//爆発の当たり判定を出す。
		AttackCollision* damage = INSTANCE(GameObjectManager)->AddNew<AttackCollision>("damage", 0);
		damage->transform->SetPosition(item->transform->GetPosition());
		SphereCollider* sphere = damage->AddComponent<SphereCollider>();
		sphere->Create(60.0f);
		DamageCollision::DamageCollisonInfo info;
		info.Damage = Random::Range(20, 45);
		info.Blown = item->GetMove() + Vector3(0, 20, 0);
		info.Blown.Normalize();
		info.Blown.Scale(180);
		info.Rigor = 1;
		float life = 0.2f;
		damage->Create(sphere, item->GetOwner(), Collision_Attribute::DAMAGE, life, info);

		//爆発したのでオブジェクトはなくなる
		INSTANCE(GameObjectManager)->AddRemoveList(item);
	};

	//関数ポインタの配列
	void(*ItemActionArray[ITEM_MAX_NUM])(Item* item, vector<Collision*> colls) =
	{
		&Standard,
		&Bom
	};
}

Item::Item(const char * name) :
	GameObject(name),
	_ItemID(-1),
	_LifeTime(10.0f),
	_Timer(0.0f),
	_Move(Vector3::zero),
	_IsHave(false),
	_ItemHandleMat(nullptr),
	_HandMat(nullptr)
{

}

Item::~Item()
{
	
}

void Item::Awake()
{
	SkinModel* model = AddComponent<SkinModel>();
	_Anim = AddComponent<Animation>();

	BoxCollider* coll = AddComponent<BoxCollider>();
	_Rigid = AddComponent<RigidBody>();

	//アイテムの種類決定
	_ItemID = Random::Range(0, ITEM_MAX_NUM - 1);
	_ItemType = itemList[_ItemID].itemtype;
	coll->Create(itemList[_ItemID].collisionsize);
	//フィルター設定
	_Rigid->SetFilter((short)Collision_Filter::ITEM, (short)fbCollisionFilterE::ALLFILTER - (short)Collision_Filter::DAMAGE);
	//コリジョン生成
	_Rigid->Create(1, coll, Collision_Attribute::ITEM , Vector3::zero, itemList[_ItemID].offset);

	//モデルセット。
	SkinModelData* modeldata = new SkinModelData();
	modeldata->CloneModelData(SkinModelManager::LoadModel(itemList[_ItemID].modelname), _Anim);
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

		//剛体にかけられている重力を無効化。
		btRigidBody* rb = (btRigidBody*)_Rigid->GetCollisonObj();
		rb->setLinearVelocity(btVector3(0.0f, 1.0f, 0.0f));
	}
	//所有されてない
	else
	{
		//削除チェック
		_DeleteCheck();
	}

	//投げられて時の処理。
	_Thrown();
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
	_DropOwner = idx;
	//投げる
	if (vec.Length() > 0)
	{
		//移動量設定
		_Move = vec;
		//アイテムタイプに合った関数取得
		_ItemAction = ItemActionArray[itemList[_ItemID].actiontype];
	}
	//置く
	else
	{
		//変更したフラグを消す。
		_Rigid->GetCollisonObj()->setCollisionFlags(0);
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

void Item::_Thrown()
{
	//移動量がある時
	if (_Move.Length() > 0.0f)
	{
		//移動前のポジション保持
		Vector3 bufpos = transform->GetPosition();
		//移動
		transform->SetPosition(bufpos + _Move);
		_Rigid->Update();
		//当たり判定(アイテム以外と当たり判定)
		vector<Collision*> colls = PhysicsWorld::Instance()->AllHitsContactTest(_Rigid, (int)fbCollisionAttributeE::ALL - (int)Collision_Attribute::ITEM - (int)Collision_Attribute::DAMAGE);

		for each (Collision* coll in colls)
		{
			//属性を確認
			if ((coll->GetCollisonObj()->getUserIndex() & Collision_Attribute::PLAYER) == 0 ||
				static_cast<Player*>(coll->gameObject)->GetIdx() != _DropOwner)
			{
				//各アイテム特有のアクションをしたい。
				_ItemAction(this, colls);
				//止まる。
				_Move = Vector3::zero;
				//変更したフラグを消す。
				_Rigid->GetCollisonObj()->setCollisionFlags(0);
				break;
			}
		}
	}
}

void Item::_DeleteCheck()
{
	//移動していない
	if (_Move.Length() <= 0.0f)
	{
		//時間がきて消滅
		_Timer += Time::DeltaTime();
		if (_Timer >= _LifeTime)
		{
			//自身を削除
			INSTANCE(GameObjectManager)->AddRemoveList(this);
		}
	}

	//一定高度以下か？
	if(transform->GetPosition().y <-200.0f)
	{
		//自身を削除
		INSTANCE(GameObjectManager)->AddRemoveList(this);
	}
}