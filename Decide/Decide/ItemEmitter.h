#pragma once
#include "fbEngine\_Object\_GameObject\GameObject.h"

class ItemEmitter:public GameObject
{
public:
	ItemEmitter(const char* name);
	~ItemEmitter();
	void Initialize(const float& interval,const Vector3& range);
	void Awake()override;
	void Update()override;
private:
	float _IntervalTime;	//¶¬üŠúe
	float _Timer;
	Vector3 _EmitRange;		//¶¬”ÍˆÍ
};