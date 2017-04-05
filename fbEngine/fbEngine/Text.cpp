#include "Text.h"
#include "FontManager.h"
#include "GameObject.h"

Text::Text(GameObject* g, Transform* t) :
	Component(g,t,typeid(this).name()),
	_WString(nullptr),
	_Size(1.0f),
	_Spacing(0),
	_Kerning(true),
	_TextFormat(fbText::TextFormatE::CENTER)
{
	
}

void Text::Awake()
{
	//スプライトを追加
	_Sprite = gameObject->AddComponent<Sprite>();
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
	Vector3 buf = transform->GetScale();
	Vector3 base = transform->GetPosition();
	transform->SetScale(Vector3(_Size, _Size, _Size));

	Vector3 pos = base;
	switch (_TextFormat)
	{
	case fbText::TextFormatE::LEFT:
		pos.x -= _Length * 0.0f;
		break;
	case fbText::TextFormatE::CENTER:
		pos.x -= _Length * 0.5f;
		break;
	case fbText::TextFormatE::RIGHT:
		pos.x -= _Length * 1.0f;
		break;
	default:
		break;
	}


	//一つ前の文字のオフセット量セット
	float offset = 0;
	//フォントをセットして文字数分描画するっていうのは重い！
	for (short i = 0;_WString[i] != '\0';i++)
	{
		//文字のデータ取得
		FontData* data = INSTANCE(FontManager)->Findfont(_WString[i], _FontStyle);
		//前の文字のoffset分ずらす
		if (_Kerning)
		{
			pos.x += (offset)* _Size;
		}
		else
		{
			pos.x += (offset + data->gm.gmptGlyphOrigin.x) * _Size;
		}
		//フォントの左上の座標を引く
		pos.y = base.y - (data->gm.gmptGlyphOrigin.y * _Size);
		transform->SetPosition(pos);
		//テクスチャセット
		_Sprite->SetTexture(data->texture);
		//描画
		_Sprite->ImageRender();
		//次の文字の開始位置
		if(_Kerning)
		{
			offset = (float)data->gm.gmBlackBoxX;
		}
		else
		{
			offset = (float)data->gm.gmCellIncX + _Spacing;
		}
		
	}
	//nullptrを設定して描画しないようにする
	_Sprite->SetTexture(nullptr);
	transform->SetPosition(base);
	transform->SetScale(buf);
}

void Text::Initialize(const wchar_t * string, const float& _Size, const Color& color, const fbSprite::SpriteEffectE& flg, const char * style, fbText::TextFormatE format)
{
	SetStyle((char*)style);
	SetSize(_Size);
	SetString(string);
	SetFormat(format);
	SetBlendColor(color);
	SetEffectFlg(flg,true);
}

void Text::SetString(const wchar_t * s)
{
	//前回の文字列解放
	if (_WString)
		SAFE_DELETE(_WString);
	//長さ(+1は終端文字分)
	size_t len = wcslen(s) + 1;
	//メモリ確保
	_WString = new wchar_t[len]();
	//値コピー
	wcscpy_s(_WString, len, s);
	INSTANCE(FontManager)->Createfont(_WString, _FontStyle);

	_UpdateLength();
}

void Text::SetStyle(const char* s)
{
	//"fbText::TextStyleE::"が含まれているかチェック
	if (strstr((char*)s, "fbText::TextStyleE::") != NULL)
	{
		strcpy_s(_FontStyle, 32, s+strlen("fbText::TextStyleE::"));
	}
	else
	{
		//そのままコピー
		strcpy_s(_FontStyle, 32, s);
	}
}

void Text::SetSize(const float& s)
{
	//作られているフォントのサイズで割った倍率
	_Size = s / FontSize;
	_UpdateLength();
}

void Text::SetSpacing(const float& space)
{
	_Spacing = space;
	_UpdateLength();
}

void Text::SetBlendColor(const Color& c)
{
	_Sprite->SetBlendColor(c);
}

void Text::SetEffectFlg(const fbSprite::SpriteEffectE& e)
{
	_Sprite->SetEffectFlg((DWORD)e);
}

void Text::SetEffectFlg(const fbSprite::SpriteEffectE& e, const bool& f)
{
	_Sprite->SetEffectFlg((DWORD)e,f);
}

void Text::SetFormat(fbText::TextFormatE format)
{
	_TextFormat = format;
}

void Text::_UpdateLength()
{
	//nullチェック
	if (!_WString)
		return;
	//初期化
	_Length = 0.0f;
	//横の長さ計算
	for (short i = 0; _WString[i] != '\0'; i++)
	{
		//文字のデータ取得
		FontData* data = INSTANCE(FontManager)->Findfont(_WString[i], _FontStyle);

		//文字の横幅を足していく
		if (_Kerning)
		{
			_Length += data->gm.gmBlackBoxX;
		}
		else
		{
			_Length += (data->gm.gmCellIncX + data->gm.gmptGlyphOrigin.x + _Spacing);
		}
	}
	//サイズをかける
	_Length *= _Size;
}
