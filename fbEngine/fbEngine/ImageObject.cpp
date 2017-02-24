#include "ImageObject.h"
#include "Sprite.h"

void ImageObject::Awake()
{
	sprite = AddComponent<Sprite>();
}

void ImageObject::SetTexture(TEXTURE * t)
{
	sprite->SetTexture(t);
}
TEXTURE * ImageObject::GetTexture()
{
	return sprite->GetTexture();
}
void ImageObject::SetBlendColor(Color c)
{
	sprite->SetBlendColor(c);
}
void ImageObject::SetClipColor(Color c)
{
	sprite->SetClipColor(c);
}

void ImageObject::SetShadow(bool f)
{
	sprite->SetShadow(f);
}
