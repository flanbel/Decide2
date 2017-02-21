#include "GameRule.h"
#include "Player.h"

GameRule::GameRule(char * name) :
	GameObject(name),
	GameSet(false),
	gamerule(GAMERULE::STOCK),
	CheckGameSet(&GameRule::StockGameSet),
	playerCount(0)
{

}

void GameRule::Awake()
{
	
}

void GameRule::Update()
{
	//ゲームが終了しているか？
	//何故か this じゃないとエラーが・・・。
	GameSet = (this->*CheckGameSet)();
}

void GameRule::SetGameRule(GAMERULE rule, int v)
{
	//設定
	this->value = v;
	//対応した関数ポインタ設定
	switch (rule)
	{
	case GameRule::STOCK:
		CheckGameSet = &GameRule::StockGameSet;
		break;
	case GameRule::TIMELIMIT:
		CheckGameSet = &GameRule::TimeLimitGameSet;
		break;
	case GameRule::KNOCKOUT:
		CheckGameSet = &GameRule::KnockOutGameSet;
		break;
	default:
		//例外ならそのまま終了
		return;
		break;
	}
	//設定
	gamerule = rule;
}

int GameRule::GetStock()
{
	if (gamerule == GAMERULE::STOCK)
		return value;
	else
		return -1;
}

void GameRule::SetPlayer(Player * p, int index)
{
	Players[index] = p;
	playerCount++;
}

void GameRule::AddKillCount(int index)
{
	Players[index]->AddKillCount();
}

void GameRule::PlayerDeath(int index)
{
	rank.push_front(index);
}

bool GameRule::StockGameSet()
{
	//最後の一人かどうか？
	return ((playerCount - 1) <= rank.size());
}

bool GameRule::TimeLimitGameSet()
{
	//制限時間が過ぎたかどうか？
	return false;
}

bool GameRule::KnockOutGameSet()
{
	//特定のプレイヤーの倒した数を見る
	return false;
}
