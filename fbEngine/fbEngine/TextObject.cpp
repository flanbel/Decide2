#include "TextObject.h"
#include "Text.h"
void TextObject::Awake()
{
	_Text = AddComponent<Text>();
}

void TextObject::Initialize(wchar_t * string, float size, Color color, DWORD flg , char * style)
{
	_Text->Initialize(string, size, color, flg, style);
}

void TextObject::SetString(wchar_t* ws)
{
	_Text->SetString(ws);
}

void TextObject::SetFontSize(float size)
{
	_Text->SetSize(size);
}

void TextObject::SetFontStyle(char* style)
{
	_Text->SetStyle(style);
}

void TextObject::SetBlendColor(Color c)
{
	_Text->SetBlendColor(c);
}

void TextObject::SetEffectFlg(DWORD f)
{
	_Text->SetEffectFlg(f);
}
