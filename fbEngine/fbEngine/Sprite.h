#pragma once
#include "Component.h"

class Vertex;

//エフェクトのフラグ
enum SpriteEffectE
{
	NONE = 0x00000000,		//なし
	SHADOW = 0x00000001,	//影描画
	MASK = 0x00000010,		//マスク処理
};

class Sprite:public Component
{
public:
	Sprite(GameObject* g, Transform* t);
	void Awake()override;
	void Start()override;
	void PreRender()override;
	void ImageRender()override;
	
	void SetTexture(TEXTURE* tex)
	{
		_Texture = tex;
	}
	TEXTURE* GetTexture()
	{
		return _Texture;
	}
	void SetPivot(Vector2 v)
	{
		_Pivot.x = min(v.x, 1.0f);
		_Pivot.y = min(v.y, 1.0f);
	}
	void SetBlendColor(Color c)
	{
		_BlendColor = c;
	}
	void SetClipColor(Color c)
	{
		_ClipColor = c;
	}
	void SetEffectFlg(DWORD f)
	{
		_SpriteEffect = (SpriteEffectE)f;
	}

private:
	//影の生成
	void _CreateShadow();
private:
	//エフェクト
	Effect* _Effect;
	//テクスチャ
	TEXTURE* _Texture;
	//基点
	Vector2 _Pivot;
	//混ぜる色
	Color _BlendColor;
	//削除する色
	Color _ClipColor;
	//スプライトにかけるエフェクトのフラグ
	SpriteEffectE _SpriteEffect;

	//頂点
	static Vertex* _Vertex;
};