#pragma once
#include "GameObject.h"

class PlatePrimitive:public GameObject
{
public:
	PlatePrimitive(char* name);
	void Awake()override;
	void Start()override;
	void Update()override;
	void Render()override;

	void SetTexture(TEXTURE* t);
	void SetPivot(Vector2 v2);
	void SetPivot(float x, float y);
	void SetBlendColor(Color c);
private:
	Effect* effect;
	//大きさ
	Vector2 size;
	//ピボット
	Vector2 pivot;
	//テクスチャー
	LPDIRECT3DTEXTURE9 texture;
	//カメラ
	Camera* camera;
	//ライト
	Light* light;
	//混ぜる色
	Color blendColor;

	//ポジションバッファー
	LPDIRECT3DVERTEXBUFFER9 postionBuffer;
	//UVバッファー
	LPDIRECT3DVERTEXBUFFER9 texcoordBuffer;
	//法線バッファー
	LPDIRECT3DVERTEXBUFFER9 normalBuffer;
	//デコレーション
	LPDIRECT3DVERTEXDECLARATION9 vertexDec;
};