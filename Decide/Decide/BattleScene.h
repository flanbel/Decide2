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
	ImageObject* test;
	TextObject *Timer;
	int playerNum;
	SoundSource *gong,*BGM;
	GameRule *gamerule;
	bool Change;
	int lastTime;	//‘O‚ÌŽžŠÔ
};