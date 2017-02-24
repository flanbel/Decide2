#include "GameRule.h"
#include "Player.h"
#include <algorithm>

GameRule::GameRule(char * name) :
	GameObject(name),
	gamerule(GAMERULE::TIMELIMIT),
	CheckGameSet(&GameRule::TimeLimitGameSet),
	playerCount(0),
	value(2),
	time(0.0f)
{

}

void GameRule::Awake()
{
	
}

void GameRule::Update()
{
	
}

void GameRule::CreateScore()
{
	//一位の人の順位が入っているか？
	if (rank.size() < playerCount)
	{
		FOR(PLAYER_NUM)
		{
			//死んだ奴はnullptrにしてあるので。
			if (Players[i] != nullptr)
			{
				PlayerDeath(i);
			}
		}	
	}

	if (gamerule == GameRule::TIMELIMIT ||
		gamerule == GameRule::KNOCKOUT)
	{
		rank.sort();
		//倒した数をキーに昇順でソート
		/*sort(rank.begin(), rank.end(),
			[](const Ranking& x, const Ranking& y) { return x.Kill < y.Kill; });*/
	}
}

void GameRule::SetGameRule(GAMERULE rule, int v)
{
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
	this->value = v;
}

GameRule::GAMERULE GameRule::GetGameRule()
{
	return gamerule;
}

int GameRule::GetValue()
{
	return value;
}

int GameRule::GetStock()
{
	if (gamerule == GAMERULE::STOCK)
		return value;
	else
		return -1;
}

int GameRule::GetRemainingTime()
{
	return max(0, (value * 60) - time);
}

bool GameRule::IsGameSet()
{
	//ゲームが終了しているか？
	//何故か this じゃないとエラーが・・・。
	return	GameSet = (this->*CheckGameSet)();
}

void GameRule::SetPlayer(Player * p, int index)
{
	Players[index] = p;
	playerCount++;
}

void GameRule::AddKillCount(int index)
{
	//死んだ相手には殺されない、ゲームセットなら加算しない。
	if (!GameSet && Players[index])
		Players[index]->AddKillCount();
}

void GameRule::PlayerDeath(int index)
{
	Ranking r = { index,Players[index]->GetKillCount() };
	rank.push_front(r);
	//ポインタを捨てる
	Players[index] = nullptr;
}

list<GameRule::Ranking>& GameRule::GetRank()
{
	return rank;
}

bool GameRule::StockGameSet()
{
	//最後の一人かどうか？
	return ((playerCount - 1) <= rank.size());
}

bool GameRule::TimeLimitGameSet()
{
	//制限時間が過ぎたかどうか？
	time += Time::DeltaTime();
	return ((value * 60) <= time);
}

bool GameRule::KnockOutGameSet()
{
	//特定のプレイヤーの倒した数を見る
	FOR(PLAYER_NUM)
	{
		if (Players[i] == nullptr)
			continue;
		if(Players[i]->GetKillCount() >= value)
		{
			return true;
		}
	}
	return false;
}
