#pragma once
#include "Collision.h"
class Collider;
//他のコリジョンと衝突しないコリジョン。
//自身と重なったコリジョンを素早く取得できる(はず)。
class GostCollision :public Collision
{
public:
	GostCollision(GameObject* g, Transform* t) :Collision(g, t, typeid(this).name())
	{
		
	};
	GostCollision(GameObject* g, Transform* t,const char* classname) :Collision(g, t, classname)
	{

	};
	virtual ~GostCollision();
	void Awake()override;
	void Create(Collider* shape, int id);
	void Update()override;
	//ゴーストオブジェクトと重なっているコリジョンを取得する。
	btAlignedObjectArray<btCollisionObject*> GetPairCollisions();
protected:
	//ゴーストへのポインタ(_CollisionObjectをキャストしただけ。)
	btGhostObject* _GostObject;
};