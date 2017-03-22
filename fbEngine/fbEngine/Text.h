#pragma once
#include "Component.h"
#include "Sprite.h"

namespace fbText
{
	//テキストの書式
	enum class TextFormatE : unsigned int
	{
		LEFT,		//左揃え
		CENTER,		//中央揃え
		RIGHT		//右揃え
	};
	//よく使うかもしれないフォントのスタイル
	enum class TextStyleE
	{
		ＭＳ_明朝,
		DEFALT,
		NewDokabenFont,		//ドカベンフォント
	};
}

//文字列を描画するだけのクラス
class Text : public Component
{
public:
	Text(GameObject* g, Transform* t);
	void Awake()override;
	void PreRender()override;
	void ImageRender()override;
	//フォントスタイルを文字列で指定
	void Initialize(const wchar_t* string, const float& size, const Color& color = Color::white,
		const fbSprite::SpriteEffectE& flg = fbSprite::SpriteEffectE::NONE, const char* style = "ＭＳ 明朝",fbText::TextFormatE format = fbText::TextFormatE::CENTER);
	//文字列セット
	void SetString(const wchar_t* s);
	//フォントのサイズ
	void SetSize(const float& s);
	//間隔設定
	void SetSpacing(const float& space);
	//文字スタイルセット
	void SetStyle(const char* s);
	//ブレンドする色
	void SetBlendColor(const Color& c);
	//エフェクトフラグセット
	void SetEffectFlg(const fbSprite::SpriteEffectE& e);
	void SetEffectFlg(const fbSprite::SpriteEffectE& e,const bool& f);
	//書式設定
	void SetFormat(fbText::TextFormatE format);
private:
	//テキストの長さ更新
	void _UpdateLength();
private:
	//制作する文字列
	wchar_t* _WString;
	//テキストの横の長さ
	float _Length;
	//サイズ
	float _Size;
	//文字の間隔
	float _Spacing;
	//文字のスタイル名デフォルトは　ＭＳ 明朝.
	char _FontStyle[32];

	//文字を詰めるかどうか？
	bool _Kerning;
	//テキストのフォーマット
	fbText::TextFormatE _TextFormat;

	//描画用スプライト
	Sprite* _Sprite;
};