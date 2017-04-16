#pragma once
#include "fbEngine/GameObject.h"
#include "DamageCollision.h"

class AttackCollision :public GameObject
{
public:
	AttackCollision(const char* name) :GameObject(name) {}
	void Awake()override;
	void Create(Collider* shape,const int& id, float& life, DamageCollision::DamageCollisonInfo& info);
	
private:
	DamageCollision* _Collision;
};