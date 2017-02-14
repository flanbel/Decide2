#pragma once
#include "fbEngine/Scene.h"
class ImageObject;
class Player;
class SoundSource;;
class BattleScene : public Scene
{
public:
	BattleScene() {};
	void Start()override;
	void Update()override;
	void LateUpdate()override;
	void Render()override;

	void SetPlayer(vector<Player*> players);
private:
	ImageObject* test;
	Player* playerArray[PLAYER_NUM];
	list<Player*> rankList;
	int playerNum;
	SoundSource *gong,*BGM;
	bool Change;
};