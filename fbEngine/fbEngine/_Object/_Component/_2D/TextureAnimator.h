#pragma once
#include "_Object\_Component\Component.h"

//テクスチャをアニメーションさせるためのUVを計算する
class TextureAnimator :public Component
{
public:
	TextureAnimator(GameObject* g, Transform* t);
	void Awake()override;
	void Update()override;

	//アニメーション再生
	//float 再生時間
	//int ループ数(負数は∞)
	//int 再生開始枚数
	void Play(const float& time ,const int& loop = -1, const int& start = 0);
	//再生一時停止
	void Pause();
	//画像の分割数設定
	//int　アニメーションの横の枚数
	//int　アニメーションの縦の枚数
	//int　アニメーションの枚数
	void SetSplit(const int& X, const int& Y, const int& max);
	//uvを取得
	const Vector4& GetUV();
	const bool& GetEnd();
private:
	//UV計算
	void _CalcUV();
private:
	//再生中
	bool _isPlaying;
	//再生終了した
	bool _End;
	//ループ数
	int _LoopNum;
	//loopカウント
	int _LoopCount;
	//縦と横の分割数
	Vector2 _Division;
	//0～1に正規化した時の1区間の縦幅横幅
	double _WidthX,_WidthY;
	//UVxyはuをzwはvを表す。
	Vector4 _UV;
	//今が何枚目のアニメーションか？
	int _NowAnimNum;
	//アニメーションの最大枚数
	int _MaxAnimNum;
	//再生時間(何秒かけて再生するか？)
	float _PlaybackTime;
	//1枚あたりの時間
	float _TimePerSheet;
	//タイマー
	float _Timer;
};