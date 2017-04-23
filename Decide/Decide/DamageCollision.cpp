#include "DamageCollision.h"
#include "Player.h"
#include "fbEngine\_Object\_GameObject\AnimationPlate.h"

void DamageCollision::Update()
{
	//ポジションの更新
	GostCollision::Update();
	//寿命確認
	_CheckDeath();
}

void DamageCollision::LateUpdate()
{
	//当たったかどうかチェック
	_CheckHit();
}

void DamageCollision::Create(const int& owner, const float& life, const int & id, Collider * shape, DamageCollisonInfo & in)
{
	_OwnerIdx = owner;
	_Life = life;
	_Timer = 0.0f;
	info = in;
	_FilterGroup = (short)Collision_Filter::DAMAGE;
	_FilterMask = (short)Collision_Filter::PLAYER;
	GostCollision::Create(shape, id);
}

void DamageCollision::_CheckDeath()
{
	_Timer += (float)Time::DeltaTime();
	if (_Life > 0.0f &&
		_Timer > _Life)
	{
		//削除
		INSTANCE(GameObjectManager)->AddRemoveList(gameObject);
		return;
	}
}

void DamageCollision::_CheckHit()
{
	//あたったかどうか？
	bool ishit = false;
	//当たっているやつを取得
	btAlignedObjectArray<btCollisionObject*> pairs = GetPairCollisions();
	FOR(i,pairs.size())
	{
		//あたったものがプレイヤーかどうか？
		if((pairs[i]->getUserIndex() & Collision_Attribute::PLAYER) != 0)
		{
			Collision* coll = (Collision*)pairs[i]->getUserPointer();
			//アップキャスト
			Player* player = (Player*)coll->gameObject;
			//自分の攻撃には当たらない。
			if (player->GetIdx() != _OwnerIdx)
			{
				//ヒットエフェクト生成
				AnimationPlate *hit = INSTANCE(GameObjectManager)->AddNew<AnimationPlate>("HitEffect", 4);
				hit->transform->SetPosition(coll->GetOffsetPos());

				hit->SetTexture(LOADTEXTURE("HitB.bmp"));
				hit->SetSplit(4, 3, 4 * 3);

				hit->SetSize(Vector2(128, 128));
				hit->SetBlendMode(fbEngine::BlendModeE::Add);
				hit->Play(0.35f, 1);
				hit->SetBlendColor(Color::yellow * 3.0f);
				hit->SetDelete(true);

				//プレイヤーにダメージを与える。
				player->Damage(_OwnerIdx, info.Damage, info.Blown, info.Rigor);

				ishit = true;
			}
		}
	}
	if (ishit)
	{
		INSTANCE(GameObjectManager)->AddRemoveList(gameObject);
	}
}