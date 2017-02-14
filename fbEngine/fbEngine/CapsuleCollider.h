/*!
 * @brief	カプセルコライダー。
 */

#pragma once

#include "Collider.h"

class CCapsuleCollider : public Collider
{
public:
	/*!
	 * @brief	コンストラクタ。
	 */
	CCapsuleCollider(GameObject* g, Transform* t);
	/*!
	 * @brief	デストラクタ。
	 */
	~CCapsuleCollider();
	/*!
	* @brief	作成。
	*/
	void Create(float radius, float height)
	{
		shape = new btCapsuleShape(radius, height);
	}
	btCollisionShape* GetBody() override
	{
		return shape;
	}
private:
	btCapsuleShape*		shape;
};