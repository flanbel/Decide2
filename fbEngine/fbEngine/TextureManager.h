#pragma once

struct TEXTURE :Noncopyable
{
	IDirect3DTexture9* pTexture;	//テクスチャへのポインタ
	Vector2 size;					//画像サイズ
	Vector2 pivot;					//基点

	//コンストラクタ
	TEXTURE::TEXTURE()
	{
		pTexture = nullptr;
		pivot = Vector2(0.0f, 0.0f);
	}

	//テクスチャへのポインタをリリースする
	void TEXTURE::Release()
	{
		SAFE_DELETE(pTexture);
	}

};	//テクスチャ関係の情報が入っている構造体

//すべての2Dテクスチャを管理
class TextureManager
{
public:
	~TextureManager();

	//外部から使う用
	//引数として受け取ったファイル名からテクスチャの情報を取得し返す関数
	//画像ファイルはImageフォルダに入っている物のみ読み込める
	//引数:char* 画像ファイルの名前(拡張子含む)
	static TEXTURE* LoadTexture(char* filename);
private:
	//テクスチャを格納するmap
	static map<UINT64,TEXTURE*> textureMap;
};