#include "ImageObject.h"

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
void ImageObject::SetPivot(const Vector2& v)
{
	_Sprite->SetPivot(v);
}
void ImageObject::SetPivot(const float& x, const float& y)
{
	_Sprite->SetPivot(Vector2(x, y));
}
void ImageObject::SetBlendColor(const Color& c)
{
	_Sprite->SetBlendColor(c);
}
void ImageObject::SetClipColor(const Color& c)
{
	_Sprite->SetClipColor(c);
}
void ImageObject::SetEffectFlg(const fbSprite::SpriteEffectE& e)
{
	_Sprite->SetEffectFlg((DWORD)e);
}

void ImageObject::SetEffectFlg(const fbSprite::SpriteEffectE& e, bool f)
{
	_Sprite->SetEffectFlg((DWORD)e,f);
}
