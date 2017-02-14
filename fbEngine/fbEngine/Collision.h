#pragma once
#include "Component.h"
class Collider;
//あたり判定の基底クラス
class Collision:public Component
{
public:
	Collision(GameObject* g, Transform* t) :Component(g, t) 
	{
		name = (char*)typeid(*this).name();
	};
	virtual ~Collision();
	void Create(btCollisionObject* collision,Collider* shape,int id);
	void Awake()override;
	virtual void Update() = 0;
	btCollisionObject* GetCollisonObj()
	{
		return collisionObject.get();
	}
	void SetHit(bool h)
	{
		isHit = h;
	}
	bool GetHit()
	{
		return isHit;
	}
protected:
	//当たっているかどうか。
	bool isHit;
	//コリジョンオブジェクト。
	std::shared_ptr<btCollisionObject>	collisionObject;
	//コリジョンの形状。
	Collider*	shape;
};