#pragma once
#include "fbEngine/GostCollision.h"

class DamageCollision:public GostCollision
{
public:
	//情報
	struct DamageCollisonInfo
	{
	public:
		Vector3 blown;		//吹き飛び
		int damage;			//威力
		float rigor;		//硬直時間
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