#pragma once
#include "fbEngine/GameObject.h"
#include "DamageCollision.h"

class BoxCollider;
class DamageCollision;

class AttackCollision :public GameObject
{
public:
	AttackCollision(char* name) :GameObject(name) {}
	void Initialize(int ID,float Life,Vector3 Size,DamageCollision::DamageCollisonInfo info);
	void Awake()override;
	void Update()override;
	
private:
	BoxCollider* shape;
	DamageCollision* coll;
};