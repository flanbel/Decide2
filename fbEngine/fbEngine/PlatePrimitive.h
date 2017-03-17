#pragma once
#include "GameObject.h"

class Vertex;

class PlatePrimitive :public GameObject
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
	Effect* _Effect;
	//大きさ
	Vector2 _Size;
	//ピボット
	Vector2 _Pivot;
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