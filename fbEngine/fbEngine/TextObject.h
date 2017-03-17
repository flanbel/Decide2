#pragma once
#include "GameObject.h"
#include "Text.h"

//テキストを表示するゲームオブジェクト
class TextObject :public GameObject
{
public:
	TextObject(char* name) :GameObject(name) {}
	void Awake()override;
	//一括で設定
	void Initialize(const wchar_t* string, const float& size,
		const Color& color = Color::white, const sprite::SpriteEffectE& flg = sprite::SpriteEffectE::NONE, const char* style = "ＭＳ 明朝", TEXT::TextFormatE format = TEXT::TextFormatE::CENTER);
	void SetString(const wchar_t* ws);
	void SetFontSize(const float& size);
	void SetFontStyle(const char* style);
	void SetBlendColor(const Color& c);
	void SetEffectFlg(const sprite::SpriteEffectE& e);
	void SetEffectFlg(const sprite::SpriteEffectE& e, const bool& f);
	//書式設定
	void SetFormat(TEXT::TextFormatE format);
private:
	Text* _Text;
};
