#pragma once
#include "fbEngine/GameObject.h"
#include "DamageCollision.h"

class BoxCollider;
class DamageCollision;

class AttackCollision :public GameObject
{
public:
	AttackCollision(const char* name) :GameObject(name) {}
	void Awake()override;
	void Update()override;
	void Create(const int& ID, float& Life, Vector3& Size, DamageCollision::DamageCollisonInfo& info);
	
private:
	BoxCollider* shape;
	DamageCollision* coll;
};