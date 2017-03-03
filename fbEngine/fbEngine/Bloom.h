#pragma once

class Effect;
class ImageObject;
class Sprite;
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
	void UpdateWeight(float dis);

private:
	/** 有効フラグ. */
	bool isEnable_;

	/** 重み. */
	static const int NUM_WEIGHTS = 8;
	/** 重み [num weights]. */
	float Weights_[NUM_WEIGHTS];

	/** エフェクト. */
	Effect* Effect;

	/** レンダリングターゲット. */
	RenderTarget* LuminanceRT;

	/** ぼかし合成用のRT. */
	RenderTarget* CombineRT;

	/** ダウンサンプリング用RTの数. */
	static const int NUM_DOWN_SAMPLING_RT = 10;
	/** 輝度をダウンサンプリングするためのRT. */
	RenderTarget* DownSamplingRT[NUM_DOWN_SAMPLING_RT];
};