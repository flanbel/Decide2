#pragma once

//文字のデータ
struct FontData :Noncopyable
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
	FontManager();
	~FontManager();
	//文字のテクスチャ生成
	//戻り値：なし
	//第一引数：wchar_t* テクスチャを生成したい文字列
	//第二引数：TCHAR* 文字のスタイル
	void Createfont(const wchar_t* string,const  char* Style);
	//文字検索
	FontData* Findfont(const wchar_t& wchar,const char* Style);
	
	static FontManager* Instance()
	{
		if (_Instance == nullptr)
		{
			_Instance = new FontManager();
		}
		return _Instance;
	}
private:
	FontData* _CreateFontTexture(const int& code);
	FontData* _CreateOutLineFontTexture(const wchar_t& wc,const int& code);
private:
	//デバイスコンテキスト
	HDC _HDC;
	//フォントの属性
	LOGFONT _LFont;
	//論理フォントのハンドル
	HFONT _HFont;
	//色の階調
	int _Grad;
	int _GradFlg;
	//文字を管理するマップの定義
	typedef std::map<wchar_t, FontData*> FontMap;
	//マップの配列
	std::map<int, FontMap*> _FontMapArray;
	static FontManager* _Instance;
};