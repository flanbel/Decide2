#pragma once
#include "Component.h"
#include "Sprite.h"

//文字列を描画するだけのクラス
class Text : public Component
{
public:
	Text(GameObject* g, Transform* t);
	void Awake()override;
	void PreRender()override;
	void ImageRender()override;
	void Initialize(const wchar_t* string, const float& size, const Color& color = Color::white,
		const sprite::SpriteEffectE& flg = sprite::SpriteEffectE::NONE, const char* style = "ＭＳ 明朝",TEXT::TextFormatE format = TEXT::TextFormatE::CENTER);
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
	void SetEffectFlg(const sprite::SpriteEffectE& e);
	void SetEffectFlg(const sprite::SpriteEffectE& e,const bool& f);
	//書式設定
	void SetFormat(TEXT::TextFormatE format);
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
	TEXT::TextFormatE _TextFormat;

	//描画用スプライト
	Sprite* _Sprite;
};