#include "Text.h"
#include "FontManager.h"
#include "GameObject.h"
#include "Sprite.h"

Text::Text(GameObject* g, Transform* t) :
	Component(g,t)
{
	name = (char*)typeid(*this).name();
	
}

void Text::Awake()
{
	//スプライトを追加
	sprite = gameObject->AddComponent<Sprite>();
	BlendColor = Color::black;
	sprite->SetBlendColor(BlendColor);
	//削除する色
	sprite->SetClipColor(Color(1.0f, 1.0f, 1.0f, 0.0f));
	strcpy_s(fontStyle, 32, "ＭＳ 明朝");
	size = 1.0f;
}

void Text::PreRender()
{

}

void Text::ImageRender()
{
	Vector3 buf = transform->scale;
	Vector3 base = transform->position;
	transform->scale = Vector3(size, size, size);
	//一つ前の文字のオフセット量セット
	float offset = 0;
	//フォントをセットして文字数分描画するっていうのは重い！
	for (short i = 0;ws[i] != '\0';i++)
	{
		//文字のデータ取得
		FontData* data = FontManager::Findfont(ws[i], fontStyle);
		//前の文字のoffset分ずらす
		transform->position.x += offset;
		//フォントサイズ*0.7がちょうどいい感じ?
		transform->position.y = (base.y - (data->gm.gmBlackBoxY*size)) + ((FontSize*0.7) * size);
		//テクスチャセット
		sprite->SetTexture(data->texture);
		//描画
		sprite->ImageRender();
		//次の文字のオフセット位置決定
		offset = data->gm.gmBlackBoxX * size;
	}
	//nullptrを設定して描画しないようにする
	sprite->SetTexture(nullptr);
	transform->scale = buf;
}

void Text::Initialize(wchar_t * string, float size, Color color, bool shadow, char * style)
{
	SetStyle(style);
	SetString(string);
	SetSize(size);
	SetBlendColor(color);
	SetShadowFlg(shadow);
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
	BlendColor = c;
	sprite->SetBlendColor(BlendColor);
}

void Text::SetShadowFlg(bool f)
{
	sprite->SetShadow(f);
}
