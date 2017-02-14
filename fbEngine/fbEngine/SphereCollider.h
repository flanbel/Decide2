/*!
 * @brief	球体コライダー。
 */

#pragma once

#include "Collider.h"

class SphereCollider : public Collider
{
public:
	/*!
	 * @brief	コンストラクタ。
	 */
	SphereCollider(GameObject* g, Transform* t);
	/*!
	 * @brief	デストラクタ。
	 */
	~SphereCollider();
	/*!
	 * @brief	球体コライダーを作成。
	 */
	void Create( const float radius );
	btCollisionShape* GetBody() override
	{
		return shape;
	}
private:
	btSphereShape*		shape;
};