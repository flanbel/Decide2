#include "TextureManager.h"

map<UINT64, TEXTURE*> TextureManager::_TextureMap;

//関数の前方定義
HRESULT SetTexture(TEXTURE& tex, char* path);

TextureManager::~TextureManager()
{
	map<UINT64, TEXTURE*>::iterator it = _TextureMap.begin();

	while (it != _TextureMap.end())
	{
		//テクスチャ解放
		it->second->Release();
	}
}

//登録、読み込みを行う
TEXTURE* TextureManager::LoadTexture(char* filename)
{
	TEXTURE* tex;

	UINT64 hush = Support::MakeHash(filename);
	//一致するものなし
	if (_TextureMap[hush] == nullptr)
	{
		tex = new TEXTURE();
		char path[128] = "Asset/Texture/";
		//パスの作成
		strcat_s(path, strlen(path) + strlen(filename)+1, filename);

		//テクスチャ情報読み込みが成功すれば登録
		if (SUCCEEDED(SetTexture(*tex,path)))
		{
			//mapに登録
			_TextureMap[hush] = tex;
		}
		//読み込み失敗(画像がないとか)
		else
		{
			string p = path;
			string error = "画像パス：" + p + " の画像の読み込みに失敗しました。";
			
			MessageBoxA(NULL, error.c_str(), "error:画像読み込みエラー", MB_OK);
		}
	}
	else
	{
		tex = _TextureMap[hush];
	}

	return tex;
}

HRESULT SetTexture(TEXTURE& tex,char* path)
{
	//テクスチャの情報
	D3DXIMAGE_INFO info;
	D3DXGetImageInfoFromFileA(path,&info);
	D3DCAPS9 cap;
	(*graphicsDevice()).GetDeviceCaps(&cap);
	PALETTEENTRY palette[256];
	
	////情報取得
	//D3DXGetImageInfoFromFile(
	//	path,	//テクスチャパス
	//	&info	//情報格納先
	//);
	////読み込み
	//hr = D3DXCreateTextureFromFile(
	//	graphicsDevice(),	//グラフィックデバイス
	//	path,				//テクスチャパス
	//	&tex.pTexture		//テクスチャの格納先
	//);

	//テクスチャ読込　各情報をセット(画像のサイズとか)
	HRESULT hr = D3DXCreateTextureFromFileEx(
		graphicsDevice(),			//グラフィックデバイスへのポインタ
		path,						//ファイルパス
		D3DX_DEFAULT,				//幅、D3DX_DEFAULT ならファイルから取得
		D3DX_DEFAULT,				//高さ、D3DX_DEFAULT ならファイルから取得
		D3DX_DEFAULT,				//ミップレベル？
		0,							//使い方0以外だとレンダーターゲットとして使えたりする。
		D3DFMT_UNKNOWN,				//フォーマット,D3DFMT_UNKNOWN だとファイルから取得する
		D3DPOOL_DEFAULT,			//テクスチャの配置先のメモリ指定 よくわからんからデフォで
		D3DX_FILTER_LINEAR,			//フィルタリングの指定 線形補完を指定
		D3DX_DEFAULT,				//ミップフィルター？
		0x00000000,					//透明にする色(透明な黒を指定)
		&info,						//ファイルのデータを格納するポインタ
		palette,					//パレット？？？
		&tex.pTexture);				//作成されたテクスチャへのポインタ

	// テクスチャサイズの取得
	tex.Size = Vector2((float)info.Width, (float)info.Height);

	return hr;
}
