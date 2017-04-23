#include "Collision.h"

#ifdef _DEBUG
#include "_Object\_GameObject\ModelOject.h"
#endif // _DEBUG


Collision::~Collision()
{
	_Shape = nullptr;
	//シェアードポインタなのでnullを入れるだけでOK
	_CollisionObject = nullptr;
#ifdef _DEBUG
	if(_CollisionModel)
	INSTANCE(GameObjectManager)->AddRemoveList(_CollisionModel);
#endif // _DEBUG
}
void Collision::Awake()
{
	_Offset = Vector3::zero;
	_Shape = nullptr;
	_CollisionObject = nullptr;
	_FilterGroup = (short)fbCollisionFilterE::A;
	_FilterMask = (short)fbCollisionFilterE::ALLFILTER;
}

void Collision::Update()
{
	//トランスフォームの更新
	_UpdateCollisionTrans();
}

void Collision::Create(btCollisionObject * collision, Collider * shape, const int & id, Vector3 offset)
{
	_Offset = offset;
	_Shape = shape;
	//コリジョンオブジェクト設定
	_CollisionObject.reset(collision);
	_CollisionObject->setCollisionShape(_Shape->GetBody());
	_CollisionObject->setUserPointer(this);	//コンポーネントのアドレス設定。
	_CollisionObject->setUserIndex(id);		//コリジョンID設定
	//トランスフォーム更新
	_UpdateCollisionTrans();

#ifdef _DEBUG
	//前に設定されていたアドレスを削除
	if (_CollisionModel)
		INSTANCE(GameObjectManager)->AddRemoveList(_CollisionModel);

	//形状取得
	int type = _Shape->GetBody()->getShapeType();
	//形に対応したモデル読み込み
	switch (type)
	{
	case 0:	//box
		//当たり判定を視覚化したオブジェクト生成
		_CollisionModel = INSTANCE(GameObjectManager)->AddNew<ModelObject>("ShowCollision", 10);
		_CollisionModel->LoadModel("Debug/BoxCollision.X");
		break;
	case 8:	//sphere
		//当たり判定を視覚化したオブジェクト生成
		_CollisionModel = INSTANCE(GameObjectManager)->AddNew<ModelObject>("ShowCollision", 10);
		_CollisionModel->LoadModel("Debug/SphereCollision.X");
		break;
	default:
		break;
	}
	//モデルがあるのなら
	if (_CollisionModel)
	{
		_CollisionModel->GetSkinModel()->SetModelEffect(ModelEffectE::NONE);
		//半透明な赤に設定。
		_CollisionModel->GetSkinModel()->SetAllBlend(Color(1.0f, 0.0f, 0.0f, 0.5f));
		//子に設定
		_CollisionModel->transform->SetParent(this->transform);
		//あたり判定の大きさを調べる。
		btTransform t = _CollisionObject->getWorldTransform();
		btVector3 min, max;
		_Shape->GetBody()->getAabb(t, min, max);
		Vector3 size = Vector3(max.x() - min.x(), max.y() - min.y(), max.z() - min.z());
		//当たり判定のサイズを指定
		_CollisionModel->transform->SetLocalScale(size);
		//当たり判定をずらす
		_CollisionModel->transform->SetLocalPosition(_Offset);
	}
#endif // _DEBUG
}

void Collision::SetFilter(short group, short mask)
{
	_FilterGroup = group;
	_FilterMask = mask;
}

void Collision::SetFilterGroup(short group)
{
	_FilterGroup = group;
}

void Collision::SetFilterMask(short mask)
{
	_FilterMask = mask;
}

void Collision::_UpdateCollisionTrans()
{
	//コリジョンのトランスフォームの参照を取得
	btTransform& trans = _CollisionObject->getWorldTransform();
	//移動を設定
	trans.setOrigin(btVector3(GetOffsetPos().x, GetOffsetPos().y, GetOffsetPos().z));
	//回転を設定
	trans.setRotation(btQuaternion(transform->GetRotation().x, transform->GetRotation().y, transform->GetRotation().z, transform->GetRotation().w));
}