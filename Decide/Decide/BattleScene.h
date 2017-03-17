#pragma once
#include "fbEngine/Scene.h"
class ImageObject;
class TextObject;
class Player;
class SoundSource;
class GameRule;
class BattleScene : public Scene
{
public:
	BattleScene() {};
	void Start()override;
	void Update()override;
private:
	//制限時間の表示更新
	void _UpdateTimer();
	//ゲーム開始前のカウントダウン
	//カウントダウンが終了したならtrueを返す。
	bool _CountDown();
private:
#ifdef _DEBUG
	ImageObject* _ShowDepth;
#endif //_DEBUG
	//タイマー表示テキスト
	TextObject *_GameTimerT,*_CountDownT;
	SoundSource *_Gong,*_BGM;
	GameRule *_GameRule;
	int _LastTime;	//前の時間
	double _CountDownTimer;
};