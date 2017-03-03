#pragma once
#include "Component.h"

class Sprite;

//文字列を描画するだけのクラス
class Text : public Component
{
public:
	Text(GameObject* g, Transform* t);
	void Awake()override;
	void PreRender()override;
	void ImageRender()override;
	void Initialize(wchar_t* string, float size, Color color = Color::white, DWORD flg = 0x00000000, char* style = "ＭＳ 明朝");
	//文字列セット
	void SetString(WCHAR* s);
	//文字スタイルセット
	void SetStyle(char* s);
	void SetSize(float s);
	void SetBlendColor(Color c);
	void SetEffectFlg(DWORD f);
private:
	//制作する文字列
	WCHAR* _WString;
	//サイズ
	float _Size;
	//色を保持
	Color _BlendColor;
	//文字のスタイル名デフォルトは　ＭＳ 明朝
	char _FontStyle[32];

	//描画用スプライト
	Sprite* _Sprite;
};