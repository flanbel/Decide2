#include "GameRule.h"
#include "Player.h"
#include <algorithm>	//sort

GameRule::GameRule(const char * name) :
	GameObject(name),
	_PlayerCount(0),
	_DeathCount(0),
	_Timer(0.0f),
	_GameState(GameStateE::BEFORE_GAME)
{
	SetGameRule(GameRuleE::TIMELIMIT, 3);
}

void GameRule::Awake()
{
	_GameState = GameStateE::BEFORE_GAME;
}

void GameRule::Update()
{
	//ゲーム中のみカウントを進める
	if (_GameState == GameStateE::DURING_GAME)
	{
		//制限時間が過ぎたかどうか？
		_Timer += Time::DeltaTime();
	}
}

void GameRule::SetGameRule(const GameRuleE& rule, const int& v)
{
	//対応した関数ポインタ設定
	switch (rule)
	{
	case GameRule::STOCK:
		CheckGameSet = &GameRule::_StockGameSet;
		break;
	case GameRule::TIMELIMIT:
		CheckGameSet = &GameRule::_TimeLimitGameSet;
		break;
	case GameRule::KNOCKOUT:
		CheckGameSet = &GameRule::_KnockOutGameSet;
		break;
	default:
		//例外ならそのまま終了
		return;
		break;
	}
	//設定
	_GameRule = rule;
	this->_Value = v;
}

const GameRule::GameRuleE& GameRule::GetGameRule()
{
	return _GameRule;
}

const int& GameRule::GetValue()
{
	return _Value;
}

const int& GameRule::GetStock()
{
	if (_GameRule == GameRuleE::STOCK)
		return _Value;
	else
		return -1;
}

int GameRule::GetRemainingTime()
{
	return max(0, (_Value * 60) - _Timer);
}

const GameRule::GameStateE & GameRule::IsGameSet()
{
	//何故か this じゃないとエラーが・・・。
	bool gameset = (this->*CheckGameSet)();
	//ゲーム中かゲーム後か設定
	_GameState = (gameset) ? GameStateE::AFTER_GAME : GameStateE::DURING_GAME;
	return	_GameState;
}

const GameRule::GameStateE& GameRule::GetGameState()
{
	return	_GameState;
}

void GameRule::SetPlayer(Player * p, const int& index)
{
	_Players[index] = p;
	Ranking rank = { 1,index,0,0 };
	_Ranking.push_front(rank);
	_PlayerCount++;
}

void GameRule::UpdateScore(const int& index)
{
	//誰かに殺された　&& ゲームセットではない
	if (index > -1 && _GameState != GameStateE::AFTER_GAME)
	{
		_Players[index]->AddKillCount();
	}
	//ランキング更新
	_UpdateRanking();
}

void GameRule::PlayerDeath(const int& index)
{
	//死んだ数を増やす
	_DeathCount++;
}

const list<GameRule::Ranking>& GameRule::GetRanking()
{
	return _Ranking;
}

void GameRule::_UpdateRanking()
{
	//情報更新
	list<Ranking>::iterator it = _Ranking.begin();
	while (it != _Ranking.end())
	{
		//プレイヤーから情報を取得
		Player* p = _Players[it->Idx];
		it->Kill = p->GetKillCount();
		it->Stock = p->GetStockCount();
		it++;
	}

	//ルールに沿ったソートをする。
	if(_GameRule == GameRuleE::STOCK)
	{
		_Ranking.sort(Ranking::StockSort());
	}
	else
	{
		_Ranking.sort(Ranking::KillSort());
	}

	//順位付け
	it = _Ranking.begin();
	int rank = 1;
	Ranking Before = { -1,-1,-1,-1 };	//前のやつ
	//全員分まわす
	while (it != _Ranking.end())
	{
		//順位計算
		if(_GameRule == GameRuleE::STOCK)
		{
			//残機が同じなら
			if (Before.Stock == it->Stock && it->Stock > 0)
			{
				//順位を同じに
				rank = Before.Rank;
			}
		}
		else
		{
			//倒した数が同じなら
			if (Before.Kill == it->Kill)
			{
				//順位を同じに
				rank = Before.Rank;
			}
		}
		
		//順位更新
		it->Rank = rank;
		Player* player = _Players[it->Idx];
		//一位なら
		//(一番最初に自殺した場合全員一位になってしまう。)
		if(it->Rank == 1)
		{
			player->No1(true);
		}else
		{
			//一位じゃない
			player->No1(false);
		}
		//保持
		Before = (*it);
		rank++;
		it++;
	}
}

bool GameRule::_StockGameSet()
{
	//最後の一人かどうか？
	return ((_PlayerCount - 1) <= _DeathCount);
}

bool GameRule::_TimeLimitGameSet()
{
	return ((_Value * 60) <= _Timer);
}

bool GameRule::_KnockOutGameSet()
{
	//特定のプレイヤーの倒した数を見る
	FOR(idx,PLAYER_NUM)
	{
		if (_Players[idx] == nullptr)
			continue;
		if(_Players[idx]->GetKillCount() >= _Value)
		{
			return true;
		}
	}
	return false;
}
