#pragma once
#include "GameObject.h"
#include "_Object\_Component\_3D\PlatePrimitive.h"

class Plate :public GameObject
{
public:
	Plate(char* name);
	void Awake()override;

	void SetBillboard(const bool& f);
	void SetBlendMode(fbEngine::BlendModeE mode);
	void SetTexture(TEXTURE* t);
	void SetSize(const Vector2& size);
	void SetPivot(const Vector2& v2);
	void SetPivot(float x, float y);
	void SetUV(const Vector4& uv);
	void SetBlendColor(const Color& c);
protected:
	PlatePrimitive* _Primitive;
};