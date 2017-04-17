#include "TextObject.h"

void TextObject::Awake()
{
	_Text = AddComponent<Text>();
}

void TextObject::Initialize(const wchar_t * string, const float& size, const Color& color, const fbSprite::SpriteEffectE& flg , const char * style, fbText::TextFormatE format)
{
	_Text->Initialize(string, size, color, flg, style, format);
}

void TextObject::SetString(const wchar_t* ws)
{
	_Text->SetString(ws);
}

void TextObject::SetFontSize(const float& size)
{
	_Text->SetSize(size);
}

void TextObject::SetFontStyle(const char* style)
{
	_Text->SetStyle(style);
}

void TextObject::SetBlendColor(const Color& c)
{
	_Text->SetBlendColor(c);
}

void TextObject::SetEffectFlg(const fbSprite::SpriteEffectE& e)
{
	_Text->SetEffectFlg(e);
}

void TextObject::SetEffectFlg(const fbSprite::SpriteEffectE& e, const bool& f)
{
	_Text->SetEffectFlg(e,f);
}

void TextObject::SetFormat(fbText::TextFormatE format)
{
	_Text->SetFormat(format);
}
