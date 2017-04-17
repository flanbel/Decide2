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
		(LONG)FontSize,	//文字の高さ 0だとデフォ -だとデフォから引かれる	//大きめに生成して後で倍率を変える
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
	{
		//TODO:: フォントを使いまわす処理
		int a = 0;
	}
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
			FontData* data = _CreateOutLineFontTexture(string[i],code);

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
	//デバイスコンテキストからフォントの情報を取得
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
			//α計算
			DWORD alpha = pMono[y * fontWidth + x] * 255 / _Grad;
			//フォントのデフォルトカラー(透明な白色)
			DWORD color = D3DCOLOR_ARGB(0, 255, 255, 255);
			//αを計算して、テクスチャへの書き込み
			pTexBuf[y * fontWidth + x] = (alpha << 24) | color;
		}
	}

	tex->pTexture->UnlockRect(0);  // アンロック
	SAFE_DELETE_ARRAY(pMono);
	//サイズを設定
	tex->Size = Vector2(fontWidth, fontHeight);
	//データ生成してポインタを返す。
	return new FontData(tex, gm);
}

FontData * FontManager::_CreateOutLineFontTexture(const wchar_t & wc, const int & code)
{
	TEXTMETRICA tm;
	GLYPHMETRICS gm;
	MAT2 mat = { { 0,1 },{ 0,0 },{ 0,0 },{ 0,1 } };
	GetTextMetricsA(_HDC, &tm);
	//GLYPHMETRICS のみ取得
	GetGlyphOutlineW(_HDC, code, GGO_METRICS, &gm, 0, 0, &mat);
	static float pensize = 6.0f;

	//半角スペースだけ特殊に(全角は知らん。)
	if (code == ' ')
	{
		gm.gmBlackBoxX = gm.gmCellIncX;
	}


	RECT charRegion = {     // LT - RB
		gm.gmptGlyphOrigin.x - pensize / 2,
		tm.tmAscent - gm.gmptGlyphOrigin.y - pensize / 2,
		gm.gmptGlyphOrigin.x + gm.gmBlackBoxX + pensize / 2,
		tm.tmAscent - gm.gmptGlyphOrigin.y + gm.gmBlackBoxY + pensize / 2
	};

	RECT charWH = { 0, 0, (gm.gmBlackBoxX + pensize ) , (gm.gmBlackBoxY + pensize ) };
	int bmpW = charWH.right;
	int bmpH = charWH.bottom;
	//ビットマップ
	BITMAPINFO bmpInfo = {};
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biWidth = bmpW;
	bmpInfo.bmiHeader.biHeight = -bmpH;
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biBitCount = 24;
	//ビットマップへのアドレス
	unsigned char *bmp = 0;
	HBITMAP hBitMap = CreateDIBSection(0, &bmpInfo, DIB_RGB_COLORS, (void**)&bmp, 0, 0);
	HBITMAP oldBMP = (HBITMAP)SelectObject(_HDC, hBitMap);

	// BMP背景を青色で初期化
	HBRUSH bgBrush = (HBRUSH)CreateSolidBrush(RGB(0, 0, 255));
	//塗りつぶし。
	FillRect(_HDC, &charWH, bgBrush);
	DeleteObject(bgBrush);

	// パス色は緑、塗は赤
	HPEN   hPen = (HPEN)CreatePen(PS_SOLID, pensize, RGB(0, 255, 0));
	HBRUSH hBrush = (HBRUSH)CreateSolidBrush(RGB(255, 0, 0));
	HPEN   oldPen = (HPEN)SelectObject(_HDC, hPen);
	HBRUSH oldBrush = (HBRUSH)SelectObject(_HDC, hBrush);

	SetBkMode(_HDC, TRANSPARENT);
	BeginPath(_HDC);
	TextOutW(_HDC, -charRegion.left, -charRegion.top, &wc, 1);
	EndPath(_HDC);
	StrokeAndFillPath(_HDC);

	SelectObject(_HDC, oldPen);
	SelectObject(_HDC, oldBrush);
	//SelectObject(_HDC, oldFont);
	SelectObject(_HDC, oldBMP);
	DeleteObject(hBrush);
	DeleteObject(hPen);
	//DeleteObject(hFont);

	// DirectXのテクスチャにBMPの色分けを使いフォントを穿つ
	int texW = charWH.right;
	int texH = charWH.bottom ;
	// テクスチャ作成
	TEXTURE* tex = new TEXTURE();
	(*graphicsDevice()).CreateTexture(texW, texH, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &tex->pTexture, 0);
	D3DLOCKED_RECT lockR;
	//テクスチャロック
	if (SUCCEEDED(tex->pTexture->LockRect(0, &lockR, 0, 0))) {
		//ビット取得
		char *d = (char*)lockR.pBits;
		unsigned BMPPitch = (charWH.right * 3 + 3) / 4 * 4;
		//テクスチャに書き込むループ
		for (int y = 0; y < texH; y++) {
			for (int x = 0; x < texW; x++) {
				//ここに書き込めばいい？
				unsigned &v = *((unsigned*)d + x + y * texW);   // テクスチャのピクセル位置
				unsigned alph = 0;
				unsigned edge = 0;
				unsigned fill = 0;

				alph = bmp[y * BMPPitch + x * 3 + 0];
				edge = bmp[y * BMPPitch + x * 3 + 1];
				fill = bmp[y * BMPPitch + x * 3 + 2];

				// 透過度がある場合はエッジ色を採用
				// 不透明の場合はブレンド色を採用
				Color edgeC, fillC;
				edgeC = Color::black;
				fillC = Color::white;
				unsigned a = 0xff - alph;
				if (a < 0xff) {
					// 半透明
					unsigned r = (edgeC.r * 255);
					unsigned g = (edgeC.g * 255);
					unsigned b = (edgeC.b * 255);
					a = (a * 255) >> 8;
					v = a << 24 | r << 16 | g << 8 | b;
				}
				else {
					// 不透明
					unsigned r = ((int)(edgeC.r * 255 * edge) >> 8) + ((int)(fillC.r * 255 * fill) >> 8);
					unsigned g = ((int)(edgeC.g * 255 * edge) >> 8) + ((int)(fillC.r * 255 * fill) >> 8);
					unsigned b = ((int)(edgeC.b * 255 * edge) >> 8) + ((int)(fillC.r * 255 * fill) >> 8);
					a = ((255 * edge) >> 8) + ((255 * fill) >> 8);
					v = a << 24 | r << 16 | g << 8 | b;
				}
			}
		}
		tex->pTexture->UnlockRect(0);
	}

	DeleteObject(hBitMap);
	//サイズを設定
	tex->Size = Vector2(texW, texH);
	//データ生成してポインタを返す。
	return new FontData(tex, gm);
}
