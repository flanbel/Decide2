#include "Text.h"
#include "FontManager.h"
#include "GameObject.h"
#include "Sprite.h"

Text::Text(GameObject* g, Transform* t) :Component(g,t)
{
	name = (char*)typeid(*this).name();
	
}

void Text::Awake()
{
	//スプライトを追加
	sprite = gameObject->AddComponent<Sprite>();
	sprite->SetBlendColor(Color::black);
	sprite->SetClipColor(Color(1.0f, 1.0f, 1.0f, 0.0f));
	strcpy_s(fontStyle, 32, "ＭＳ 明朝");
	size = Vector3::one;
}

void Text::ImageRender()
{
	Vector3 buf = transform->scale;
	Vector3 base = transform->position;
	transform->scale = size;
	//一つ前の文字のオフセット量セット
	float offset = 0;
	//フォントをセットして文字数分描画するっていうのは重い！
	for (short i = 0;ws[i] != '\0';i++)
	{
		FontData* data = FontManager::Findfont(ws[i],fontStyle);
		//とりあえず右に
		transform->position.x += offset;
		//フォントサイズ*0.7がちょうどいい感じ?
		transform->position.y = (base.y - (data->gm.gmBlackBoxY*size.y)) + ((FontSize*0.7) * size.y);
		sprite->SetTexture(data->texture);
		sprite->ImageRender();
		offset = data->gm.gmBlackBoxX * size.x;
	}
	//nullptrを設定して描画しないようにする
	sprite->SetTexture(nullptr);
	transform->scale = buf;
}

void Text::SetString(WCHAR * s)
{
	//前回の文字列解放
	if (ws)
		SAFE_DELETE(ws);
	//長さ(+1は終端文字分)
	size_t len = wcsnlen(s, 128) + 1;
	//メモリ確保
	ws = new wchar_t[len]();
	//値コピー
	wcscpy_s(ws, len, s);
	FontManager::Createfont(ws, fontStyle);
}

void Text::SetStyle(char * s)
{
	strcpy_s(fontStyle, 32, s);
}

void Text::SetBlendColor(Color c)
{
	sprite->SetBlendColor(c);
}