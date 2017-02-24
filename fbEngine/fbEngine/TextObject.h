#pragma once
#include "GameObject.h"

class Text;
//テキストを表示するゲームオブジェクト
class TextObject :public GameObject
{
public:
	TextObject(char* name) :GameObject(name) {}
	void Awake()override;
	void Initialize(wchar_t* string, float size, Color color = Color::white, bool shadow = false, char* style = "ＭＳ 明朝");
	void SetString(wchar_t* ws);
	void SetFontSize(float size);
	void SetFontStyle(char* style);
	void SetBlendColor(Color c);
	void SetShadowFlg(bool f);
private:
	Text* text;
};
