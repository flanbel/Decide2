#pragma once
#include "fbEngine/Scene.h"
#include "GameRule.h"
class Player;
class ResultScene : public Scene
{
public:
	ResultScene() {};
	void Start()override;
	void Update()override;

	void CreateRanking(list<GameRule::Ranking> ranking);
private:
	GameRule *gamerule;
};