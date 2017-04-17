#pragma once
#include "_Object\_Component\Component.h"

class ModelObject;
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
	virtual void Update();
	//コリジョン生成
	//第一引数 btCollisionObject* バレットフィジックスのコリジョンオブジェクト
	//第二引数 Collider* あたり判定の形状
	//第三引数 const int コリジョンに設定するID
	//第四引数 Vector3& 基点からの移動量
	void Create(btCollisionObject* collision, Collider* shape, const int& id,Vector3 offset = Vector3::zero);
	btCollisionObject* GetCollisonObj() const
	{
		return _CollisionObject.get();
	}
	//オフセットした先のポジション取得
	const Vector3 GetOffsetPos()
	{
		return transform->GetPosition() + _Offset;
	}
	//Createより前に設定しといて。
	void SetFilter(short group, short mask);
	void SetFilterGroup(short group);
	void SetFilterMask(short mask);
protected:
	//コリジョンの位置や回転を更新
	void _UpdateCollisionTrans();
protected:
	//ポジションからのオフセット量
	Vector3 _Offset;
	//自分の属するグループ
	short _FilterGroup;
	//当たり判定をとるグループ
	short _FilterMask;
	//コリジョンの形状。
	Collider *_Shape;
	//コリジョンオブジェクト。
	std::shared_ptr<btCollisionObject>	_CollisionObject;
private:
	//当たり判定を視覚化した3Dオブジェクト。
	ModelObject* _CollisionModel;
};