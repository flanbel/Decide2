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
private:
	void _CreateRanking(const list<GameRule::Ranking>& ranking);
private:
	GameRule *_GameRule;
};