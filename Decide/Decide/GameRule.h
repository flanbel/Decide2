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
		NUM,
	};
	//ランキング(仮)
	struct Ranking {
	public:
		int rank;//順位
		int idx;  // プレイヤーの添え字
		int Kill; // 倒した数
		int Stock;

		//キル数でのソート
		struct KillSort
		{
			bool operator()(const Ranking& left, const Ranking& right) const
			{
				//キル数で降順
				if (left.Kill > right.Kill)
				{
					return true;
				}
				//同数なら添え字の小さいほうを上に
				else if (left.Kill == right.Kill && left.idx < right.idx)
				{
					return true;
				}
				//並び替えしない
				return false;
			}
		};
		struct StockSort
		{
			//ストック数でソート
			bool operator()(const Ranking& left, const Ranking& right) const
			{
				//ストックで降順
				if (left.Stock > right.Stock)
				{
					return true;
				}
				//並び替えしない
				return false;
			}
		};
	};

	
	GameRule(char* name);
	void Awake()override;
	void Update()override;
	//スコア生成こいつの仕事か？
	void CreateScore();
	//ゲームルール設定
	//第二引数はそれぞれのルールに対応したもの（ストック数,制限時間(分),KO数）
	void SetGameRule(GAMERULE rule,int value);
	//ルール取得
	GameRule::GAMERULE GetGameRule();
	//値取得
	int GetValue();
	//ストック数取得
	int GetStock();
	//残り時間取得
	int GetRemainingTime();
	//ゲームが終了しているか否か
	bool IsGameSet();


	//プレイヤーセット
	void SetPlayer(Player* p,int index);
	//プレイヤーのスコア更新
	void UpdateScore(int index);
	//プレイヤー完全に死んだ通知を受け取る
	void PlayerDeath(int index);
	list<Ranking>& GetRank();
private:
	//ランキング更新
	void UpdateRanking();
	
private:
	//プレイヤー達を管理？
	Player* Players[PLAYER_NUM];
	//何人ﾌﾟﾚｲしているか？
	int playerCount;
	//何人死んだか？
	int DeathCount;
	//順位
	list<Ranking> rank;

	bool GameSet;
	//ゲームルール
	GAMERULE _GameRule;
	//ストック、制限時間、KO数のいずれかが入る。
	int value;
	//大変不本意ではあるがtimeを計る用の変数
	float time;


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