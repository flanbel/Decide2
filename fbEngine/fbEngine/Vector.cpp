/*!
 * @brief	ベクトルクラス。
 */

#include "Vector.h"


const Vector3 Vector3::zero	= {  0.0f,  0.0f,  0.0f };
const Vector3 Vector3::right	= {  1.0f,  0.0f,  0.0f };
const Vector3 Vector3::left	= { -1.0f,  0.0f,  0.0f };
const Vector3 Vector3::up		= {  0.0f,  1.0f,  0.0f };
const Vector3 Vector3::down	= {  0.0f, -1.0f,  0.0f };
const Vector3 Vector3::front	= { 0.0f,   0.0f,  1.0f };
const Vector3 Vector3::back	= { 0.0f,   0.0f, -1.0f };

const Vector3 Vector3::axisX	= { 1.0f,  0.0f,  0.0f };
const Vector3 Vector3::axisY	= { 0.0f,  1.0f,  0.0f };
const Vector3 Vector3::axisZ	= { 0.0f,  0.0f,  1.0f };
const Vector3 Vector3::one = { 1.0f, 1.0f, 1.0f };
const Quaternion Quaternion::Identity = { 0.0f,  0.0f, 0.0f, 1.0f };