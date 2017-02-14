#include "FontManager.h"

std::map<int, FontManager::FontMap*> FontManager::fontMapArray;

void FontManager::Createfont(wchar_t* string, char* Style)
{
	// フォントの生成準備
	//大きめに生成して後で倍率を変える
	//フォントの属性を定義
	LOGFONT lf = { 
		FontSize,	//文字の高さ 0だとデフォ -だとデフォから引かれる
		0,	//横幅 0だと立て幅に合わせてくれる
		0,	//文字送りの方向？
		0,	//傾き？
		FW_BOLD,	//文字の太さ　0がデフォ　マクロ定義がある(FW_)
		0,	//イタリック体になるかどうかのフラグ
		0,	//下線フラグ
		0,	//打消し線フラグ
		SHIFTJIS_CHARSET,			//キャラクタセット
		OUT_TT_ONLY_PRECIS,			//出力精度
		CLIP_DEFAULT_PRECIS,		//クリッピング精度
		CLEARTYPE_QUALITY,		//出力品質
		DEFAULT_PITCH | FF_MODERN,	//ピッチとファミリア指定
		NULL //フォントスタイル32byteまで
	};
	//フォントスタイルはコピー
	strcpy_s(lf.lfFaceName, 32, Style);
	//ウィンドウズのAPIでフォント作成
	HFONT hFont = CreateFontIndirect(&lf);

	//デバイスコンテキスト取得
	HDC hdc = GetDC(NULL);
	// デバイスにフォントを持たせないとGetGlyphOutline関数はエラーとなる
	HFONT oldFont = (HFONT)SelectObject(hdc, hFont);
	

	const int gradFlag = GGO_GRAY4_BITMAP; // GGO_GRAY2_BITMAP or GGO_GRAY4_BITMAP or GGO_GRAY8_BITMAP
	int grad = 0; // 階調の最大値
	switch (gradFlag) {
	case GGO_GRAY2_BITMAP: grad = 4; break;
	case GGO_GRAY4_BITMAP: grad = 16; break;
	case GGO_GRAY8_BITMAP: grad = 64; break;
	}

	int hash = MakeHash(Style);
	//スタイルのmapが登録されているか？
	std::map<int, FontManager::FontMap*>::iterator array = fontMapArray.find(hash);
	if (array == fontMapArray.end())
	{
		//されていないので登録
		array = fontMapArray.insert(std::make_pair(hash, new FontMap)).first;
	}

	FontManager::FontMap* map = array->second;

	//文字生成開始
	short i = 0;
	for (short i = 0; string[i] != '\0'; i++)
	{
		//文字を文字コードに変換？
		int code = (int)string[i];
		//mapないに文字が登録されているか
		if (map->find(code) == map->end())
		{
			//登録されていない
			TEXTMETRIC tm;
			GetTextMetrics(hdc, &tm);
			GLYPHMETRICS gm;
			CONST MAT2 mat = { { 0, 1 }, { 0, 0 }, { 0, 0 }, { 0, 1 } };
			DWORD size = GetGlyphOutlineW(hdc, code, gradFlag, &gm, 0, NULL, &mat);
			BYTE *pMono = new BYTE[size];
			GetGlyphOutlineW(hdc, code, gradFlag, &gm, size, pMono, &mat);

			// テクスチャ作成
			TEXTURE* Tex = new TEXTURE();
			int fontWidth = (gm.gmBlackBoxX + 3) / 4 * 4;
			int fontHeight = gm.gmBlackBoxY;
			(*graphicsDevice()).CreateTexture(fontWidth, fontHeight, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &Tex->pTexture, NULL);

			// テクスチャにフォントビットマップ情報を書き込み
			D3DLOCKED_RECT lockedRect;
			Tex->pTexture->LockRect(0, &lockedRect, NULL, 0);  // ロック
			DWORD *pTexBuf = (DWORD*)lockedRect.pBits;   // テクスチャメモリへのポインタ

			for (int y = 0; y < fontHeight; y++) {
				for (int x = 0; x < fontWidth; x++) {
					DWORD alpha = pMono[y * fontWidth + x] * 255 / grad;
					//テクスチャへの書き込み？
					pTexBuf[y * fontWidth + x] = (alpha << 24) | D3DCOLOR_ARGB(0, 255, 255, 255);// 0x00ffffff;透明な白を繰り抜く
				}
			}

			Tex->pTexture->UnlockRect(0);  // アンロック
			SAFE_DELETE_ARRAY(pMono);
			//TEXTURE作成
			Tex->size = Vector2(fontWidth, fontHeight);

			FontData* data = new FontData(Tex,gm);

			//マップに追加
			map->insert(std::make_pair(code, data));
		}
	}
	//フォントのテクスチャを作ったならいらないので解放
	DeleteObject(hFont);
	DeleteDC(hdc);
}

FontData* FontManager::Findfont(wchar_t wchar, char* Style)
{
	int hash = MakeHash(Style);
	FontMap* map = fontMapArray[hash];
	if(map)
	{
		return map->at(wchar);
	}
	return nullptr;
}