#pragma once
#include "Component.h"
class Collider;
//あたり判定の基底クラス
class Collision:public Component
{
public:
	Collision(GameObject* g, Transform* t) :
		Component(g, t, typeid(this).name())
	{
	
	};
	Collision(GameObject* g, Transform* t,const char* classname) :
		Component(g, t, classname)
	{
		
	};
	virtual ~Collision();
	void Awake()override;
	virtual void Update() = 0;
	//コリジョン生成
	//第一引数 btCollisionObject* バレットフィジックスのコリジョンオブジェクト
	//第二引数 Collider* あたり判定の形状
	//第三引数 const int コリジョンに設定するID
	//第四引数 Vector3& 基点からの移動量
	void Create(btCollisionObject* collision, Collider* shape, const int& id,Vector3 offset = Vector3::zero);
	btCollisionObject* GetCollisonObj()
	{
		return _CollisionObject.get();
	}
	void SetHit(const bool& h)
	{
		_IsHit = h;
	}
	const bool& GetHit()
	{
		return _IsHit;
	}
protected:
	//コリジョンの位置や回転を更新
	void _UpdateCollisionTrans();
protected:
	//当たっているかどうか。
	bool _IsHit;
	//基点からのオフセット量
	Vector3 _Offset;
	//コリジョンオブジェクト。
	std::shared_ptr<btCollisionObject>	_CollisionObject;
	//コリジョンの形状。
	Collider *_Shape;
};