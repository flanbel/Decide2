/*!
 * @brief	ベクトル
 */

#pragma once


//二次元ベクトル
class Vector2 {
public:
	//共用体
	union {
		struct { float x, y; };
		float v[2];
	};

	Vector2() 
	{
		this->x = this->y = 0;
	}

	Vector2(float x, float y)
	{
		Set(x, y);
	}
	
	void Set(float x, float y)
	{
		this->x = x;
		this->y = y;
	}

	/*!
	* @brief	長さを取得
	*/
	float Length()
	{
		return sqrt(LengthSq());
	}
	/*!
	* @brief	長さの二乗を取得
	*/
	float LengthSq()
	{
		return x * x + y * y;
	}

	Vector2 operator + (Vector2 v)
	{
		this->x += v.x;
		this->y += v.y;
	}

	Vector2 operator - (Vector2 v)
	{
		this->x -= v.x;
		this->y -= v.y;
		return *this;
	}

	Vector2 operator * (Vector2 v)
	{
		this->x *= v.x;
		this->y *= v.y;
	}

	Vector2 operator * (float v)
	{
		Vector2 vec2;
		vec2.x = this->x * v;
		vec2.y = this->y * v;
		return vec2;
	}

	Vector2 operator / (Vector2 v)
	{
		this->x /= v.x;
		this->y /= v.y;
	}

	Vector2 operator / (float v)
	{
		Vector2 vec2;
		vec2.x = this->x / v;
		vec2.y = this->y / v;
		return vec2;
	}
};

//三次元ベクトル
class Vector3{
public:
	union{
		struct { float x, y, z; };
		float v[3];
	};
	static const Vector3 zero;
	static const Vector3 right;
	static const Vector3 left;
	static const Vector3 up;
	static const Vector3 down;
	static const Vector3 front;
	static const Vector3 back;
	static const Vector3 axisX;
	static const Vector3 axisY;
	static const Vector3 axisZ;
	static const Vector3 one;
public:
	operator D3DXVECTOR3() { return static_cast<D3DXVECTOR3>(*this); }
	operator LPVOID() { return static_cast<LPVOID>(this); }
	
	Vector3() 
	{
		this->x = this->y = this->z = 0;
	}
	//D3DXVECTOR3をVector3に変換
	Vector3(D3DXVECTOR3& v)
	{
		this->x = v.x;
		this->y = v.y;
		this->z = v.z;
	}
	/*!
	* @brief	コンストラクタ。
	*/
	Vector3(float x, float y, float z)
	{
		Set(x, y, z);
	}
	//Vector3をD3DXVECTOR3に変換コピーする
	void CopyFrom(D3DXVECTOR3& v) const
	{
		v.x = x;
		v.y = y;
		v.z = z;
	}
	template<class TVector>
	void CopyTo(TVector& dst) const
	{
		dst.x = x;
		dst.y = y;
		dst.z = z;
	}
	/*!
	* @brief	ベクトルの各要素を設定。
	*/
	void Set(float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}
	template<class TVector>
	void Set(TVector& dst)
	{
		this->x = dst.x;
		this->y = dst.y;
		this->z = dst.z;
	}
	/*!
	 * @brief	ベクトルを加算。
	 */
	void Add(const Vector3& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
	}
	void Add(const Vector3& v0, const Vector3& v1)
	{
		x = v0.x + v1.x;
		y = v0.y + v1.y;
		z = v0.z + v1.z;
	}
	/*!
	 * @brief	ベクトルを減算。
	 */
	void Subtract(const Vector3& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
	}
	void Subtract(const Vector3& v0, const Vector3& v1)
	{
		x = v0.x - v1.x;
		y = v0.y - v1.y;
		z = v0.z - v1.z;
	}
	/*!
	 * @brief	内積。
	 */
	float Dot(const Vector3& v)
	{
		return x * v.x + y * v.y + z * v.z;
	}
	/*!
	 * @brief	外積。
	 */
	void Cross(const Vector3& v)
	{
		float _x = (x * v.z) - (v.y * z);
		float _y = (z * v.x) - (v.z * x);
		float _z = (x * v.y) - (v.x * y);
		x = _x;
		y = _y;
		z = _z;
	}
	void Cross(const Vector3& v0, const Vector3& v1)
	{
		float _x = (v0.y * v1.z) - (v1.y * v0.z);
		float _y = (v0.z * v1.x) - (v1.z * v0.x);
		float _z = (v0.x * v1.y) - (v1.x * v0.y);
		x = _x;
		y = _y;
		z = _z;
	}
	/*!
	 * @brief	長さを取得
	 */
	float Length()
	{
		return sqrt(LengthSq());
	}
	/*!
	 * @brief	長さの二乗を取得
	 */
	float LengthSq()
	{
		return x * x + y * y + z * z;
	}
	/*!
	* @brief	拡大。
	*/
	void Scale(float s)
	{
		x *= s;
		y *= s;
		z *= s;
	}
	/*!
	* @brief	法線を正規化。
	*/
	void Normalize()
	{
		float len = Length();
		x /= len;
		y /= len;
		z /= len;
	}
	/*!
	* @brief	除算。
	*/
	void Div(float d)
	{
		x /= d;
		y /= d;
		z /= d;
	}

	Vector3 operator + (Vector3 v)
	{
		this->x += v.x;
		this->y += v.y;
		this->z += v.z;
		return *this;
	}

	Vector3 operator - (Vector3 v)
	{
		Vector3 out;
		out.x = this->x - v.x;
		out.y = this->y - v.y;
		out.z = this->z - v.z;
		return out;
	}

	Vector3 operator * (Vector3 v)
	{
		this->x *= v.x;
		this->y *= v.y;
		this->z *= v.z;
	}

	Vector3 operator / (Vector3 v)
	{
		this->x /= v.x;
		this->y /= v.y;
		this->z /= v.z;
	}
};
	
//四次元ベクトル
class Vector4{
public:
	union{
		struct { float x, y, z, w; };
		float v[4];
	};
public:
	operator D3DXVECTOR4(void) { return static_cast<D3DXVECTOR4>(*this); }
	Vector4(){}
	/*!
	 *@brief	コンストラクタ
	 */
	Vector4(float x, float y, float z, float w)
	{
		Set(x, y, z, w);
	}
	/*!
	 *@brief	ベクトルの各要素を設定。
	 */
	void Set(float x, float y, float z, float w)
	{
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}
	/*!
	 *@brief	ベクトルを加算。
	 */
	void Add(const Vector4& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		w += v.w;
	}
	void Add(const Vector4& v0, const Vector4& v1)
	{
		x = v0.x + v1.x;
		y = v0.y + v1.y;
		z = v0.z + v1.z;
		w = v0.w + v1.w;
	}
	/*!
	 *@brief	ベクトルを減算。
	 */
	void Subtract(const Vector4& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
	}
	void Subtract(const Vector4& v0, const Vector4& v1)
	{
		x = v0.x - v1.x;
		y = v0.y - v1.y;
		z = v0.z - v1.z;
		w = v0.w - v1.w;
	}
	/*!
	 *@brief	内積
	 */
	float Dot(const Vector4& v)
	{
		return x * v.x + y * v.y + z * v.z + w * v.w;
	}
	/*!
	 * @brief	長さを取得
	 */
	float Length()
	{
		return sqrt(LengthSq());
	}
	/*!
	 * @brief	長さの二乗を取得
	 */
	float LengthSq()
	{
		return x * x + y * y + z * z + w * w;
	}
	/*!
	* @brief	拡大。
	*/
	void Scale(float s)
	{
		x *= s;
		y *= s;
		z *= s;
		w *= s;
	}

	Vector4 operator + (Vector4 v)
	{
		this->x += v.x;
		this->y += v.y;
		this->z += v.z;
		this->w += v.w;
	}

	Vector4 operator - (Vector4 v)
	{
		this->x -= v.x;
		this->y -= v.y;
		this->z -= v.z;
		this->w -= v.w;
	}

	Vector4 operator * (Vector4 v)
	{
		this->x *= v.x;
		this->y *= v.y;
		this->z *= v.z;
		this->w *= v.w;
	}

	Vector4 operator / (Vector4 v)
	{
		this->x /= v.x;
		this->y /= v.y;
		this->z /= v.z;
		this->w /= v.w;
	}
};
	
class Quaternion : public Vector4{
public:
	static const Quaternion Identity;		//!<単位クォータニオン。
	Quaternion() 
	{
		this->x = this->y = this->z = 0.0f;
		this->w = 1.0f;
	}
	Quaternion(float x, float y, float z, float w) :
		Vector4(x, y, z, w)
	{
	}
	//D3DXVECTOR3をVector3に変換
	Quaternion(D3DXQUATERNION& q)
	{
		this->x = q.x;
		this->y = q.y;
		this->z = q.z;
		this->w = q.w;
	}

	/*!
	 *@brief	任意の軸周りの回転クォータニオンを作成。
	 */
	void SetRotation(const Vector3& axis, float angle)
	{
		float s;
		float halfAngle = angle * 0.5f;
		s = sin(halfAngle);
		w = cos(halfAngle);
		x = axis.x * s;
		y = axis.y * s;
		z = axis.z * s;
	}
	/*!
	*@brief	クォータニオン同士の積。
	*/
	void Multiply(const Quaternion& rot)
	{
		float pw, px, py, pz;
		float qw, qx, qy, qz;

		pw = w; px = x; py = y; pz = z;
		qw = rot.w; qx = rot.x; qy = rot.y; qz = rot.z;

		w = pw * qw - px * qx - py * qy - pz * qz;
		x = pw * qx + px * qw + py * qz - pz * qy;
		y = pw * qy - px * qz + py * qw + pz * qx;
		z = pw * qz + px * qy - py * qx + pz * qw;

	}
	void CopyFrom(D3DXQUATERNION& q)
	{
		q.x = x;
		q.y = y;
		q.z = z;
		q.w = w;
	}
};