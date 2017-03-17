#pragma once

class Effect;
class Vertex;
struct RenderTarget;

class Bloom
{
public:
	Bloom(){}
	~Bloom(){}

	void Create();
	void Render();
	void Release();

private:
	void _UpdateWeight(const float& dis);

private:
	//
	Vertex* _Vertex;
	//有効フラグ. 
	bool _IsEnable;

	//重みの数
	static const int NUM_WEIGHTS = 8;
	//重み
	float _Weights[NUM_WEIGHTS];

	//エフェクト
	Effect* _Effect;

	//輝度用のレンダリングターゲット
	RenderTarget* _LuminanceRT;

	//ぼかし合成用のRT
	RenderTarget* _CombineRT;

	//ダウンサンプリング用RTの数
	static const int NUM_DOWN_SAMPLING_RT = 10;
	//輝度をダウンサンプリングするためのRT
	RenderTarget* _DownSamplingRT[NUM_DOWN_SAMPLING_RT];
};