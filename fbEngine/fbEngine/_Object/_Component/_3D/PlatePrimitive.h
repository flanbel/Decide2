#pragma once
#include "_Object\_Component\Component.h"

class Vertex;
namespace fbEngine
{
	enum BlendModeE
	{
		Trans = 1,	//乗算合成
		Add,		//加算合成
	};
}
//板ポリゴンを出す
class PlatePrimitive :public Component
{
public:
	PlatePrimitive(GameObject* g, Transform* t);
	void Awake()override;
	void Start()override;
	void Update()override;
	void Render()override;

	void SetBillboard(const bool& f);
	void SetBlendMode(fbEngine::BlendModeE mode);
	void SetTexture(TEXTURE* t);
	void SetSize(const Vector2& size);
	void SetPivot(const Vector2& v2);
	void SetPivot(float x, float y);
	void SetUV(const Vector4& uv);
	void SetBlendColor(const Color& c);
private:
	//ビルボードするかどうか？
	bool _IsBillboard;
	//ブレンドモード
	fbEngine::BlendModeE _BlendMode;
	//シェーダーエフェクト
	Effect* _Effect;
	//大きさ
	Vector2 _Size;
	//ピボット
	Vector2 _Pivot;
	//UVxyはuをzwはvを表す。
	Vector4 _UV;
	//テクスチャー
	LPDIRECT3DTEXTURE9 _Texture;
	//カメラ
	Camera* _Camera;
	//ライト
	Light* _Light;
	//混ぜる色
	Color _BlendColor;

	static Vertex* _Vertex;	//頂点
};