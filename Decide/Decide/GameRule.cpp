#include "GameRule.h"
#include "Player.h"
#include <algorithm>

GameRule::GameRule(char * name) :
	GameObject(name),
	playerCount(0),
	DeathCount(0),
	value(2),
	time(0.0f)
{
	SetGameRule(GAMERULE::TIMELIMIT, 2);
}

void GameRule::Awake()
{
	
}

void GameRule::Update()
{
	
}

void GameRule::CreateScore()
{
	UpdateRanking();
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
	Ranking r = { 1,index,0,0 };
	rank.push_front(r);
	playerCount++;
}

void GameRule::UpdateScore(int index)
{
	//誰かに殺された　&& ゲームセットではない
	if (index >= 0 && !GameSet)
	{
		Players[index]->AddKillCount();
	}
	//ランキング更新
	UpdateRanking();
}

void GameRule::PlayerDeath(int index)
{
	//死んだ数を増やす
	DeathCount++;
}

list<GameRule::Ranking>& GameRule::GetRank()
{
	return rank;
}

void GameRule::UpdateRanking()
{
	//ランキング更新
	list<Ranking>::iterator it = rank.begin();
	while (it != rank.end())
	{
		Player* p = Players[it->idx];
		it->Kill = p->GetKillCount();
		it->Stock = p->GetStockCount();
		it++;
	}
	//順番にソート
	if(gamerule == GAMERULE::STOCK)
	{
		rank.sort(Ranking::StockSort());
	}
	else
	{
		rank.sort(Ranking::KillSort());
	}
	//順位付け
	it = rank.begin();
	int r = 1;
	Ranking Before = { -1,-1,-1,-1 };	//前のやつ
	while (it != rank.end())
	{
		Player* p = Players[it->idx];
		if(gamerule == GAMERULE::STOCK)
		{
			//残機が同じなら
			if (Before.Stock == it->Stock)
			{
				//順位を同じに
				r = Before.rank;
			}
		}
		else
		{
			//倒した数が同じなら
			if (Before.Kill == it->Kill)
			{
				//順位を同じに
				r = Before.rank;
			}
		}
		
		it->rank = r;
		//一位なら
		if(it->rank == 1)
		{
			p->No1(true);
		}else
		{
			//一位じゃない
			p->No1(false);
		}
		//保持
		Before = (*it);
		r++;
		it++;
	}
}

bool GameRule::StockGameSet()
{
	//最後の一人かどうか？
	return ((playerCount - 1) <= DeathCount);
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
