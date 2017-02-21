#pragma once
#include "fbEngine/Scene.h"
class CharaSelect;
class CharaRingFrame;
class ImageObject;
class GameRule;
class TextObject;

class CharaSelectScene : public Scene
{
public:
	CharaSelectScene() {};
	void Start()override;
	void Update()override;
private:
	CharaSelect* selectArray[PLAYER_NUM];
	CharaRingFrame* ringArray[CHARACTER_NUM];

	ImageObject* readyFight;
	GameRule* gameRule;
	TextObject* displayGameRule;
};