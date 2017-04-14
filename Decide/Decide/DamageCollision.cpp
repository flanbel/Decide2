#include "DamageCollision.h"

void DamageCollision::Update()
{
	//ポジションの更新
	GostCollision::Update();
	//寿命確認
	_CheckDeath();
	//当たったかどうかチェック
	_CheckHit();
	//当たっても削除
	if (_IsHit)
	{
		GameObjectManager::AddRemoveList(gameObject);
	}
}

void DamageCollision::Create(const float& life, const int & id, Collider * shape, DamageCollisonInfo & in)
{
	_Life = life;
	_Timer = 0.0f;
	info = in;
	GostCollision::Create(shape, id);
}

void DamageCollision::_CheckDeath()
{
	_Timer += (float)Time::DeltaTime();
	if (_Life > 0.0f &&
		_Timer > _Life)
	{
		//削除
		GameObjectManager::AddRemoveList(gameObject);
		return;
	}
}
#include "Player.h"
void DamageCollision::_CheckHit()
{
	btCollisionObject* coll = _CollisionObject.get();
	FOR(i,PLAYER_NUM)
	{
		//自分の攻撃は無視
		if (i == coll->getUserIndex())
			continue;

		char name[20] = "Player";
		char idx[3] = { (i + 1) + 48,'\0' };
		strcat(name, idx);
		//プレイヤーと当たったかどうか？
		const Collision* hit = INSTANCE(PhysicsWorld)->SearchCollisionByName(this, name, Collision_ID::PLAYER);
		if(hit)
		{
			//アップキャスト
			Player* p = (Player*)hit->gameObject;
			//プレイヤーにダメージを与える。
			p->Damage(coll->getUserIndex(), info.Damage, info.Blown, info.Rigor);
		}
	}
}