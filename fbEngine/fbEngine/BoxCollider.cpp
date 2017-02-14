#include "BoxCollider.h"

/*!
 * @brief	コンストラクタ。
 */
BoxCollider::BoxCollider(GameObject* g, Transform* t) :
	Collider(g,t),
	shape(NULL)
{
}
/*!
 * @brief	デストラクタ。
 */
BoxCollider::~BoxCollider()
{
	SAFE_DELETE(shape);
}
/*!
 * @brief	ボックスコライダーを作成。
 */
void BoxCollider::Create( const Vector3& size )
{
	shape = new btBoxShape(btVector3(size.x*0.5f, size.y*0.5f, size.z*0.5f));
}