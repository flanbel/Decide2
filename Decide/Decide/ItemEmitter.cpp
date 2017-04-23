#include "ItemEmitter.h"
#include "Item.h"

ItemEmitter::ItemEmitter(const char * name):
	GameObject(name),
	_IntervalTime(0.0f),
	_Timer(0.0f)
{
}

ItemEmitter::~ItemEmitter()
{
}

void ItemEmitter::Initialize(const float & interval, const Vector3 & range)
{
	_IntervalTime = interval;
	_Timer = 0.0f;
	_EmitRange = range;
}

void ItemEmitter::Awake()
{
}

void ItemEmitter::Update()
{
	_Timer += Time::DeltaTime();
	if(_Timer >= _IntervalTime)
	{
		//¶¬
		Item* item = INSTANCE(GameObjectManager)->AddNew<Item>("item", 1);
		//ˆÚ“®
		Vector3 pos = transform->GetPosition();
		pos.x += Random::Range(-_EmitRange.x,_EmitRange.x);
		pos.y += Random::Range(-_EmitRange.y, _EmitRange.y);
		pos.z += Random::Range(-_EmitRange.z, _EmitRange.z);
		item->transform->SetLocalPosition(pos);
		_Timer = _Timer - _IntervalTime;
	}
}
