#pragma once

#include "Collider.h"

/*!
 * @brief	ボックスコライダー。
 */
class BoxCollider : public Collider
{
public:
	/*!
	 * @brief	コンストラクタ。
	 */
	BoxCollider(GameObject* g, Transform* t);
	/*!
	 * @brief	デストラクタ。
	 */
	~BoxCollider();
	/*!
	 * @brief	ボックスコライダーを作成。
	 */
	void Create( const Vector3& size );
	btCollisionShape* GetBody() override
	{
		return _Shape;
	}
private:
	btBoxShape*		_Shape;
};