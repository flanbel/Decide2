#include "FontManager.h"

FontManager*  FontManager::_Instance = nullptr;

FontManager::FontManager()
{
	//NULL指定して画面全体のデバイスコンテキスト取得
	HDC hdc = GetDC(NULL);
	//メモリデバイスコンテキスト作成
	_HDC = CreateCompatibleDC(hdc);
	ReleaseDC(NULL, hdc);//もういらない。

	//フォントの属性を定義
	_LFont = {
		FontSize,	//文字の高さ 0だとデフォ -だとデフォから引かれる	//大きめに生成して後で倍率を変える
		0,			//横幅 0だと立て幅に合わせてくれる
		0,			//文字送りの方向？
		0,			//傾き？
		FW_BOLD,	//文字の太さ　0がデフォ　マクロ定義がある(FW_)
		0,			//イタリック体になるかどうかのフラグ
		0,			//下線フラグ
		0,			//打消し線フラグ
		SHIFTJIS_CHARSET,			//キャラクタセット
		OUT_TT_ONLY_PRECIS,			//出力精度
		CLIP_DEFAULT_PRECIS,		//クリッピング精度
		CLEARTYPE_QUALITY,			//出力品質
		DEFAULT_PITCH | FF_MODERN,	//ピッチとファミリア指定
		NULL						//フォントスタイル32byteまで
	};

	_GradFlg = GGO_GRAY4_BITMAP; // GGO_GRAY2_BITMAP or GGO_GRAY4_BITMAP or GGO_GRAY8_BITMAP
	_Grad = 0; // 階調の最大値
	switch (_GradFlg) {
	case GGO_GRAY2_BITMAP: _Grad = 4; break;
	case GGO_GRAY4_BITMAP: _Grad = 16; break;
	case GGO_GRAY8_BITMAP: _Grad = 64; break;
	}
}

FontManager::~FontManager()
{
	//デバイスコンテキスト削除
	DeleteDC(_HDC);
}

void FontManager::Createfont(const wchar_t* string,const char* Style)
{
	//使いまわせそう
	if (strcmp(_LFont.lfFaceName, Style) == 0)
		int a = 0;
	//フォントスタイルはコピー
	strcpy_s(_LFont.lfFaceName, 32, Style);
	//ウィンドウズのAPIでフォント作成
	//論理フォントのハンドル取得
	_HFont = CreateFontIndirect(&_LFont);
	// デバイスにフォントを持たせないとGetGlyphOutline関数はエラーとなる
	//戻り値として前に設定していたフォントの情報が帰ってくる。
	HFONT oldFont = (HFONT)SelectObject(_HDC, _HFont);

	//スタイルのマップのハッシュ
	const int stylehash = Support::MakeHash(Style);
	//スタイルのmapが登録されているか？
	std::map<int, FontManager::FontMap*>::iterator array = _FontMapArray.find(stylehash);
	if (array == _FontMapArray.end())
	{
		//見つからなかったので新しく登録
		array = _FontMapArray.insert(std::make_pair(stylehash, new FontMap)).first;
	}
	//イテレータから中身を取得(文字のマップ)
	FontManager::FontMap* map = array->second;

	//文字列の終端までループ
	for (short i = 0; string[i] != '\0'; i++)
	{
		//文字を文字コードに変換
		int code = (int)string[i];
		//mapに文字が登録されているか
		if (map->find(code) == map->end())
		{
			//登録されていないので新しく生成
			FontData* data = _CreateFontTexture(code);

			//マップに追加
			map->insert(std::make_pair(code, data));
		}
	}
	//フォントのテクスチャを作ったならいらないので解放
	DeleteObject(_HFont);
}

FontData* FontManager::Findfont(const wchar_t& wchar,const char* Style)
{
	int stylehash = Support::MakeHash(Style);
	//スタイルからフォントマップ取得
	FontMap* map = _FontMapArray[stylehash];
	if(map)
	{
		//フォントを返す
		return map->at(wchar);
	}
	return nullptr;
}

FontData * FontManager::_CreateFontTexture(const int & code)
{
	TEXTMETRIC tm;
	GetTextMetrics(_HDC, &tm);
	GLYPHMETRICS gm;
	CONST MAT2 mat = { { 0, 1 },{ 0, 0 },{ 0, 0 },{ 0, 1 } };
	//サイズが不明なので戻り値でサイズを取得
	DWORD size = GetGlyphOutlineW(_HDC, code, _GradFlg, &gm, 0, NULL, &mat);
	BYTE *pMono = new BYTE[size];
	//文字情報のポインタ取得
	GetGlyphOutlineW(_HDC, code, _GradFlg, &gm, size, pMono, &mat);
	//半角スペースだけ特殊に(全角は知らん。)
	if (code == ' ')
	{
		gm.gmBlackBoxX = gm.gmCellIncX;
	}
	// テクスチャ作成
	TEXTURE* tex = new TEXTURE();
	//4バイトアラインなビットマップフォントの横幅を得る
	int fontWidth = (gm.gmBlackBoxX + 3) / 4 * 4;
	int fontHeight = gm.gmBlackBoxY;
	//空のテクスチャ作成
	(*graphicsDevice()).CreateTexture(fontWidth, fontHeight, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &tex->pTexture, NULL);

	// テクスチャにフォントビットマップ情報を書き込み
	D3DLOCKED_RECT lockedRect;
	tex->pTexture->LockRect(0, &lockedRect, NULL, 0);  // ロック
	DWORD *pTexBuf = (DWORD*)lockedRect.pBits;   // テクスチャメモリへのポインタ

	for (int y = 0; y < fontHeight; y++) {
		for (int x = 0; x < fontWidth; x++) {
			DWORD alpha = pMono[y * fontWidth + x] * 255 / _Grad;
			//テクスチャへの書き込み？
			pTexBuf[y * fontWidth + x] = (alpha << 24) | D3DCOLOR_ARGB(0, 255, 255, 255);// 0x00ffffff;透明な白を繰り抜く
		}
	}

	tex->pTexture->UnlockRect(0);  // アンロック
	SAFE_DELETE_ARRAY(pMono);
	//サイズを設定
	tex->Size = Vector2(fontWidth, fontHeight);
	//データ生成してポインタを返す。
	return new FontData(tex, gm);
}
