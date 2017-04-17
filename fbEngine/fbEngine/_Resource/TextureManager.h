#pragma once
#include "_Object/Noncopyable.h"

//テクスチャ構造体
struct TEXTURE :Noncopyable
{
	IDirect3DTexture9* pTexture;	//テクスチャへのポインタ
	Vector2 Size;					//画像サイズ

	//コンストラクタ
	TEXTURE::TEXTURE()
	{
		pTexture = nullptr;
	}

	//テクスチャへのポインタをリリースする
	void TEXTURE::Release()
	{
		SAFE_DELETE(pTexture);
	}

};	

//読み込んだテクスチャを管理
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
	static map<UINT64,TEXTURE*> _TextureMap;
};

//テクスチャ読み込み
#define LOADTEXTURE(filename) TextureManager::LoadTexture(filename)