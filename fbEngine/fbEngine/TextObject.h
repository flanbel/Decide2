#pragma once
#include "GameObject.h"

class Text;
//テキストを表示するゲームオブジェクト
class TextObject :public GameObject
{
public:
	TextObject(char* name) :GameObject(name) {}
	void Awake()override;

	void SetString(wchar_t* ws);
	void SetFontSize(Vector3 size);
	void SetFontSize(float size);
	void SetFontStyle(char* style);
	void SetBlendColor(Color c);
private:
	Text* text;
};
