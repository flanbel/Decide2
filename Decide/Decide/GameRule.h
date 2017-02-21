#pragma once
class Player;
//ゲームの終了判定とかも
class GameRule:public GameObject
{
public:
	//ゲームルール
	enum GAMERULE
	{
		STOCK,			//ストック制	最後まで生き残った者の勝ち。
		TIMELIMIT,		//時間制限		時間内でより多く倒した者の価値。
		KNOCKOUT,		//ノックアウト　先に一定数倒した者の勝ち。
	};
	GameRule(char* name);
	void Awake()override;
	void Update()override;
	//ゲームルール設定
	//第二引数はそれぞれのルールに対応したもの（ストック数,制限時間(分),KO数）
	void SetGameRule(GAMERULE rule,int value);
	//ストック数取得
	int GetStock();



	//プレイヤーセット
	void SetPlayer(Player* p,int index);
	//プレイヤーの倒した数を増やす。(お前がするのか・・・。)
	void AddKillCount(int index);
	//プレイヤー完全に死んだ通知を受け取る
	void PlayerDeath(int index);
private:
	//プレイヤー達を管理
	Player* Players[PLAYER_NUM];
	int playerCount;
	//順位
	list<int> rank;
	//試合終了している
	bool GameSet;
	//ストック、制限時間、KO数のいずれかが入る。
	int value;
	//ゲームルール
	GAMERULE gamerule;
	//関数ポインタ
	//GameRuleクラスの関数ポインタを受け取るようにする。
	bool(GameRule::*CheckGameSet)();

	//ストック制のゲーム終了チェック
	bool StockGameSet();
	//時間制限制のゲーム終了チェック
	bool TimeLimitGameSet();
	//ノックアウト制のゲーム終了チェック
	bool KnockOutGameSet();
};