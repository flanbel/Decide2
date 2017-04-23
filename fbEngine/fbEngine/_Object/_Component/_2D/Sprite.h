#pragma once
#include "_Object\_Component\Component.h"

class Vertex;

namespace fbSprite
{
	//エフェクトのフラグ
	enum class SpriteEffectE : unsigned int
	{
		NONE = BIT(0),		//なし				00000000
		FADE = BIT(1),		//フェードフラグ	00000010
		FADEOUT = BIT(1),	//フェードアウト	00000010
		FADEIN = BIT(1) +1,	//フェードイン		00000011
		SHADOW = BIT(2),	//影描画			00000100
		OUTLINE = BIT(3),	//ふちどり			00001000
		//未実装　MASK = 16,//マスク処理		
	};
}

class Sprite:public Component
{
public:
	Sprite(GameObject* g, Transform* t);
	void Awake()override;
	void Start()override;
	void Update()override;
	void ImageRender()override;
	
	void SetTexture(TEXTURE* tex)
	{
		_Texture = tex;
		if (_Texture)
			_Size = _Texture->Size;
	}
	void SetSize(const Vector2& size)
	{
		_Size = size;
	}
	TEXTURE* GetTexture()
	{
		return _Texture;
	}
	void SetPivot(const Vector2& v)
	{
		_Pivot.x = min(v.x, 1.0f);
		_Pivot.y = min(v.y, 1.0f);
	}
	void SetUV(const Vector4& uv)
	{
		_UV = uv;
	}
	void SetBlendColor(const Color& c)
	{
		_BlendColor = c;
	}
	void SetClipColor(const Color& c)
	{
		_ClipColor = c;
	}
	//そのままセット
	void SetEffectFlg(const DWORD& e);
	//trueなら足す、falseなら引く
	//戻り値は成功したかどうか？
	bool SetEffectFlg(const DWORD& e, bool f);
	//両面描画フラグセット
	void SetDoubleSided(const bool& f)
	{
		_DoubleSided = f;
	}
private:
	//描画する
	void _RenderSprite();
	//フェードさせる
	void Fade();
	//影の生成
	void _CreateShadow();
	//ふちの生成
	void _CreateOutLine();
private:
	//エフェクト
	Effect* _Effect;
	//テクスチャ
	TEXTURE* _Texture;
	//画像サイズ
	Vector2 _Size;
	//基点
	Vector2 _Pivot;
	//UVxyはuをzwはvを表す。
	Vector4 _UV;
	//混ぜる色
	Color _BlendColor;
	//削除する色
	Color _ClipColor;
	//消える時間
	float _FadeTime;
	//タイマー
	float _FTimer;
	//消えるしきい値
	float _FadeLine;
	//スプライトにかけるエフェクトのフラグ
	DWORD _SpriteEffect;
	//両面描画
	bool _DoubleSided;

	//頂点
	static Vertex* _Vertex;
};