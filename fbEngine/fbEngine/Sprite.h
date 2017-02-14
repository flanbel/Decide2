#pragma once
#include "Component.h"

class Sprite:public Component
{
public:
	Sprite(GameObject* g, Transform* t) :Component(g, t)
	{
		name = (char*)typeid(*this).name();
	}
	void Awake()override;
	void Start()override;
	void PreRender()override;
	void ImageRender()override;
	void SetTexture(TEXTURE* t)
	{
		texture = t;
	}
	TEXTURE* GetTexture()
	{
		return texture;
	}
	void SetBlendColor(Color c)
	{
		blendColor = c;
	}
	void SetClipColor(Color c)
	{
		clipColor = c;
	}
	void SetMask(bool m)
	{
		mask = m;
	}
private:
	Effect* effect;
	TEXTURE* texture;
	//混ぜる色
	Color blendColor;
	//消す色
	Color clipColor;
	//マスクするかどうか
	bool mask;
	
	//宣言からオブジェクト生成
	static IDirect3DVertexDeclaration9 *pvertexDec;
	//ポジションバッファー
	static LPDIRECT3DVERTEXBUFFER9 postionBuffer;
	//UVバッファー
	static LPDIRECT3DVERTEXBUFFER9 texcoordBuffer;
};