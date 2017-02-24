#include "TextObject.h"
#include "Text.h"
void TextObject::Awake()
{
	text = AddComponent<Text>();
}

void TextObject::Initialize(wchar_t * string, float size, Color color, bool shadow, char * style)
{
	text->Initialize(string, size, color, shadow, style);
}

void TextObject::SetString(wchar_t* ws)
{
	text->SetString(ws);
}

void TextObject::SetFontSize(float size)
{
	text->SetSize(size);
}

void TextObject::SetFontStyle(char* style)
{
	text->SetStyle(style);
}

void TextObject::SetBlendColor(Color c)
{
	text->SetBlendColor(c);
}

void TextObject::SetShadowFlg(bool f)
{
	text->SetShadowFlg(f);
}
