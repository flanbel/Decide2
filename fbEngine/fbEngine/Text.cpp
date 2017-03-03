#include "Text.h"
#include "FontManager.h"
#include "GameObject.h"
#include "Sprite.h"

Text::Text(GameObject* g, Transform* t) :
	Component(g,t),
	_WString(nullptr),
	_Size(1.0f),
	_Spacing(0),
	_BlendColor(Color::black),
	_Kerning(true)
{
	name = (char*)typeid(*this).name();
}

void Text::Awake()
{
	//スプライトを追加
	_Sprite = gameObject->AddComponent<Sprite>();
	//ブレンドカラー設定
	_Sprite->SetBlendColor(_BlendColor);
	//削除する色
	_Sprite->SetClipColor(Color(1.0f, 1.0f, 1.0f, 0.0f));
	_Sprite->SetPivot(Vector2(0.0f, 0.0f));
	strcpy_s(_FontStyle, 32, "ＭＳ 明朝");
}

void Text::PreRender()
{

}

void Text::ImageRender()
{
	Vector3 buf = transform->scale;
	Vector3 base = transform->position;
	transform->scale = Vector3(_Size, _Size, _Size);
	//一つ前の文字のオフセット量セット
	float offset = 0;
	//フォントをセットして文字数分描画するっていうのは重い！
	for (short i = 0;_WString[i] != '\0';i++)
	{
		//文字のデータ取得
		FontData* data = FontManager::Findfont(_WString[i], _FontStyle);
		//前の文字のoffset分ずらす
		if (_Kerning)
		{
			transform->position.x += offset * _Size;
		}
		else
		{
			transform->position.x += (offset + data->gm.gmptGlyphOrigin.x) * _Size;
		}
		
		//フォントの左上の座標を引く
		transform->position.y = base.y - (data->gm.gmptGlyphOrigin.y * _Size);
		//テクスチャセット
		_Sprite->SetTexture(data->texture);
		//描画
		_Sprite->ImageRender();
		//次の文字の開始位置
		if(_Kerning)
		{
			offset = data->gm.gmBlackBoxX;
		}
		else
		{
			offset = data->gm.gmCellIncX + _Spacing;
		}
		
	}
	//nullptrを設定して描画しないようにする
	_Sprite->SetTexture(nullptr);
	transform->scale = buf;
}

void Text::Initialize(wchar_t * string, float _Size, Color color, DWORD flg, char * style)
{
	SetStyle(style);
	SetString(string);
	SetSize(_Size);
	SetBlendColor(color);
	SetEffectFlg(flg);
}

void Text::SetString(WCHAR * s)
{
	//前回の文字列解放
	if (_WString)
		SAFE_DELETE(_WString);
	//長さ(+1は終端文字分)
	size_t len = wcsnlen(s, 128) + 1;
	//メモリ確保
	_WString = new wchar_t[len]();
	//値コピー
	wcscpy_s(_WString, len, s);
	FontManager::Createfont(_WString, _FontStyle);
}

void Text::SetStyle(char * s)
{
	strcpy_s(_FontStyle, 32, s);
}

void Text::SetSize(float s)
{
	//作られているフォントのサイズで割った倍率
	_Size = s / FontSize;
}

void Text::SetSpacing(float space)
{
	_Spacing = space;
}

void Text::SetBlendColor(Color c)
{
	_BlendColor = c;
	_Sprite->SetBlendColor(_BlendColor);
}

void Text::SetEffectFlg(DWORD f)
{
	_Sprite->SetEffectFlg(f);
}