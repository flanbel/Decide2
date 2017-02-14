#include "TextObject.h"
#include "Text.h"
void TextObject::Awake()
{
	text = AddComponent<Text>();
}

void TextObject::SetString(wchar_t* ws)
{
	text->SetString(ws);
}

void TextObject::SetFontSize(Vector3 size)
{
	text->SetSize(size);
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
