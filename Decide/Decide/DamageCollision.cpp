#include "DamageCollision.h"
#include "Player.h"

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

void DamageCollision::Create(const float& life, const int & id, Collider * shape, DamageCollisonInfo & in)
{
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
	
	//攻撃したプレイヤーの添え字取得
	int atkidx = _CollisionObject->getUserIndex() - Collision_ID::DAMAGE - 1;
	//あたったかどうか？
	bool ishit = false;
	//当たっているやつを取得
	btAlignedObjectArray<btCollisionObject*> pairs = GetPairCollisions();
	FOR(i,pairs.size())
	{
		//あたったものがプレイヤーかどうか？
		if((pairs[i]->getUserIndex() & Collision_ID::PLAYER) != 0)
		{
			Collision* coll = (Collision*)pairs[i]->getUserPointer();
			//アップキャスト
			Player* player = (Player*)coll->gameObject;
			//自分の攻撃には当たらない。
			if (player->GetIdx() != atkidx)
			{
				//プレイヤーにダメージを与える。
				player->Damage(atkidx, info.Damage, info.Blown, info.Rigor);
				ishit = true;
			}
		}
	}
	if (ishit)
	{
		INSTANCE(GameObjectManager)->AddRemoveList(gameObject);
	}
}