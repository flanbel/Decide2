#pragma once
#include "_Object\_Component\_Physics\Collision.h"

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
	//他のコリジョンが接触した時に呼ばれる
	virtual void OnCollisionEnter(Collision* coll) {};
	//触れているコリジョンが離れた時に呼び出される。
	virtual void OnCollisionExit(Collision* coll) {};
protected:
	//ゴーストへのポインタ(_CollisionObjectをキャストしただけ。)
	btGhostObject* _GostObject;
};