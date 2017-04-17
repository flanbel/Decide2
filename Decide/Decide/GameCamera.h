#pragma once
#include "fbEngine/_Object\_GameObject/GameObject.h"
class Player;
//ゲームカメラ
class GameCamera :public GameObject
{
public:
	GameCamera(const char* name) :GameObject(name) {}
	void Awake()override;
	void Update()override;

	void AddPlayer(Player* p);
private:
	//画角更新
	void _UpdateViewAngle();
	//視点更新
	void _UpdatePos();
private:
	Camera* camera;
	vector<Player*> _PlayerList;
};