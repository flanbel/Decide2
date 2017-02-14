#pragma once
#include "fbEngine/Scene.h"
class Player;
class ResultScene : public Scene
{
public:
	ResultScene() {};
	void Start()override;
	void Update()override;
	void SetList();
	void Setrank(vector<int> r);
private:
	list<Player*>* rankList;
	vector<int> rank;
};