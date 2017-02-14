#pragma once
#include "Component.h"

class Sprite;

//文字列を描画するだけのクラス
class Text : public Component
{
public:
	Text(GameObject* g, Transform* t);
	void Awake()override;
	void ImageRender()override;
	//文字列セット
	void SetString(WCHAR* s);
	//文字スタイルセット
	void SetStyle(char* s);
	void SetSize(Vector3 v)
	{
		v.Scale(1.0f / FontSize);
		size = v;
	}
	void SetSize(float s)
	{
		s /= FontSize;
		size = Vector3(s, s, s);
	}
	void SetBlendColor(Color c);
private:
	//制作する文字列
	WCHAR* ws;
	//文字のスタイル名デフォルトは　ＭＳ 明朝
	char fontStyle[32];
	//描画用スプライト
	Sprite* sprite;
	//サイズ
	Vector3 size;
};