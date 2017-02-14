/*!
 * @brief	カプセルコライダー。
 */

#include "CapsuleCollider.h"


 /*!
 * @brief	コンストラクタ。
 */
CCapsuleCollider::CCapsuleCollider(GameObject* g, Transform* t) :
	Collider(g,t),
	shape(nullptr)
{
}
/*!
* @brief	デストラクタ。
*/
CCapsuleCollider::~CCapsuleCollider()
{
	delete shape;
}