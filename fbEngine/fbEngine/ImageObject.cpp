#include "ImageObject.h"
#include "Sprite.h"

void ImageObject::Awake()
{
	_Sprite = AddComponent<Sprite>();
}

void ImageObject::SetTexture(TEXTURE * t)
{
	_Sprite->SetTexture(t);
}
TEXTURE * ImageObject::GetTexture()
{
	return _Sprite->GetTexture();
}
void ImageObject::SetPivot(Vector2 v)
{
	_Sprite->SetPivot(v);
}
void ImageObject::SetPivot(float x, float y)
{
	_Sprite->SetPivot(Vector2(x, y));
}
void ImageObject::SetBlendColor(Color c)
{
	_Sprite->SetBlendColor(c);
}
void ImageObject::SetClipColor(Color c)
{
	_Sprite->SetClipColor(c);
}
void ImageObject::SetEffectFlg(DWORD f)
{
	_Sprite->SetEffectFlg(f);
}