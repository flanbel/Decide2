#pragma once
#include "_Object\_GameObject\Plate.h"
#include "_Object\_Component\_2D\TextureAnimator.h"

class AnimationPlate :public Plate
{
public:
	AnimationPlate(char* name);
	void Awake()override;
	void Update();

	//画像の分割数設定
	//int　アニメーションの横の枚数
	//int　アニメーションの縦の枚数
	//int　アニメーションの枚数
	void SetSplit(const int& X, const int& Y, const int& max);
	//アニメーション再生
	//float 再生時間
	//bool ループするか？
	//int 再生開始枚数
	void Play(const float& time, const int& loop = -1, const int& start = 0);
	//再生一時停止
	void Pause();
	void SetDelete(const bool& f);
private:	
	TextureAnimator* _Animator;
	//アニメーションが再生終了したなら勝手に削除する
	bool _AutoDelete;
};