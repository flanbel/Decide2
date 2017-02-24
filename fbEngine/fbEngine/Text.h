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
	void Initialize(wchar_t* string, float size, Color color = Color::white, bool shadow = false, char* style = "ＭＳ 明朝");
	//文字列セット
	void SetString(WCHAR* s);
	//文字スタイルセット
	void SetStyle(char* s);
	void SetSize(float s)
	{
		//作られているフォントのサイズで割った倍率
		size = s / FontSize;
	}
	void SetBlendColor(Color c);
	void SetShadowFlg(bool f);
private:
	//制作する文字列
	WCHAR* ws;
	//サイズ
	float size;
	//色を保持
	Color BlendColor;
	//文字のスタイル名デフォルトは　ＭＳ 明朝
	char fontStyle[32];

	//描画用スプライト
	Sprite* sprite;
};