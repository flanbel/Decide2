#pragma once
#include "Component.h"
#include "Sprite.h"

class SpriteAnimation:public Component
{
public:
	SpriteAnimation(GameObject* g, Transform* t);
	void Awake()override;
	void Start()override;
	void Update()override;
	void ImageRender()override;

	//アニメーション再生
	//再生時間
	//再生開始枚数
	void Play(float time ,int start = 0);
	void SetTexture(TEXTURE* tex,int X,int Y);
private:
	//スプライト
	Sprite* _Sprite;
	//再生
	bool _Play;
	//ループ
	bool _Loop;
	//縦と横の分割数
	Vector2 _Division;
	//アニメーション一枚当たりのuv
	Vector2 _UV;
	//今が何枚目のアニメーションか？
	int _NowAnimNum;
	//アニメーションの最大枚数
	int _MaxAnimNum;
	//再生時間(何秒かけて再生するか？)
	float _PlaybackTime;
	//タイマー
	float _Timer;
};