#pragma once
#include "fbEngine\_Object\_Component\_Physics\GostCollision.h"

class DamageCollision:public GostCollision
{
public:
	//攻撃判定の情報
	struct DamageCollisonInfo
	{
	public:
		Vector3 Blown;		//吹き飛び力
		int Damage;			//威力
		int StopTime;		//硬直時間
		float Rigor;		//吹き飛び時間
	};
public:
	DamageCollision(GameObject* g, Transform* t) :GostCollision(g, t,typeid(this).name())
	{
		
	};
	~DamageCollision() {};
	void Update()override;
	void LateUpdate()override;

	void Create(const float& life, const int& id, Collider* shape, DamageCollisonInfo& info);
	//げったー、セッター作るのめんどくさかった。
	DamageCollisonInfo info;
private:
	//死んだかどうかチェック
	void _CheckDeath();
	//当たったかどうか？
	void _CheckHit();
private:
	float _Life;	//寿命。0以下だと死なない
	float _Timer;
};