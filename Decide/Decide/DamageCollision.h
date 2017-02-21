#pragma once
#include "fbEngine/GostCollision.h"

class DamageCollision:public GostCollision
{
public:
	//攻撃判定の情報
	struct DamageCollisonInfo
	{
	public:
		Vector3 blown;		//吹き飛び力
		int damage;			//威力
		int stoptime;		//硬直時間
		float rigor;		//吹き飛び時間
	};

	DamageCollision(GameObject* g, Transform* t) :GostCollision(g, t)
	{
		name = (char*)typeid(*this).name();
	};
	~DamageCollision() {};
	void Create(float life,int id, Collider* shape, DamageCollisonInfo info);
	void Update()override;
	//げったー、セッター作るのめんどくさかった。
	DamageCollisonInfo info;
private:
	float life;	//寿命。0以下だと死なない
	float time;
};