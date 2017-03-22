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
	void Initialize(const wchar_t* string, const float& size,const Color& color = Color::white,
		const fbSprite::SpriteEffectE& flg = fbSprite::SpriteEffectE::NONE, const char* style = "ＭＳ 明朝", fbText::TextFormatE format = fbText::TextFormatE::CENTER);

	void SetString(const wchar_t* ws);
	void SetFontSize(const float& size);
	void SetFontStyle(const char* style);
	void SetBlendColor(const Color& c);
	void SetEffectFlg(const fbSprite::SpriteEffectE& e);
	void SetEffectFlg(const fbSprite::SpriteEffectE& e, const bool& f);
	//書式設定
	void SetFormat(fbText::TextFormatE format);
private:
	Text* _Text;
};
