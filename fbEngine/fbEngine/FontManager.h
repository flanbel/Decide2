#pragma once

//文字のデータ
struct FontData:Noncopyable
{
	FontData(TEXTURE* t, GLYPHMETRICS g)
	{
		texture = t;
		gm = g;
	}
	//文字のテクスチャ
	TEXTURE* texture;
	//文字の高さとか幅とかの情報
	GLYPHMETRICS gm;
};

class FontManager
{
public:
	//文字のテクスチャ生成
	//戻り値：なし
	//第一引数：wchar_t* テクスチャを生成したい文字列
	//第二引数：TCHAR* 文字のスタイル
	static void Createfont(wchar_t* string, char* Style);
	//文字検索
	static FontData* Findfont(wchar_t wchar,char* Style);
private:
	//文字を管理するマップの定義
	typedef std::map<wchar_t, FontData*> FontMap;
	//マップの配列
	static std::map<int, FontMap*> fontMapArray;
};