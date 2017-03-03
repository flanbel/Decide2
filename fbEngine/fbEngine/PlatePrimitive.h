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

	static Vertex* _Vertex;	//頂点
};