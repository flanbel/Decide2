/*!
 * @brief	コライダー。
 */

#pragma once
#include "Component.h"
/*!
 * @brief	コライダーのインターフェースクラス。
 */
class Collider :public Component
{
public:
	Collider(GameObject* g, Transform* t) :Component(g, t)
	{
		name = (char*)typeid(*this).name();
	};
	~Collider(){}
	virtual btCollisionShape* GetBody() = 0;
};