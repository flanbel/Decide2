#include "Plate.h"

Plate::Plate(char * name) :
	GameObject(name)
{
	
}

void Plate::Awake()
{
	_Primitive = AddComponent<PlatePrimitive>();
}

void Plate::SetBillboard(const bool & f)
{
	_Primitive->SetBillboard(f);
}

void Plate::SetBlendMode(fbEngine::BlendModeE mode)
{
	_Primitive->SetBlendMode(mode);
}

void Plate::SetTexture(TEXTURE * t)
{
	_Primitive->SetTexture(t);
}

void Plate::SetSize(const Vector2 & size)
{
	_Primitive->SetSize(size);
}

void Plate::SetPivot(const Vector2 & v2)
{
	_Primitive->SetPivot(v2);
}

void Plate::SetPivot(float x, float y)
{
	_Primitive->SetPivot(x,y);
}

void Plate::SetUV(const Vector4 & uv)
{
	_Primitive->SetUV(uv);
}

void Plate::SetBlendColor(const Color & c)
{
	_Primitive->SetBlendColor(c);
}
