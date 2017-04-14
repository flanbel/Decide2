#pragma once
class Player;
//ゲームの終了判定とかも
class GameRule:public GameObject
{
public:
	//ゲームルール
	enum GameRuleE
	{
		STOCK,			//ストック制	最後まで生き残った者の勝ち。
		TIMELIMIT,		//時間制限		時間内でより多く倒した者の価値。
		KNOCKOUT,		//ノックアウト　先に一定数倒した者の勝ち。
		NUM,
	};
	//試合の状態
	enum GameStateE
	{
		BEFORE_GAME = 0,	//試合前
		DURING_GAME,		//試合中
		AFTER_GAME,			//試合後
	};
	//
	//ランキング(仮)
	struct Ranking {
	public:
		int Rank;	//順位
		int Idx;	//プレイヤーの添え字
		int Kill;	//倒した数
		int Stock;	//残機

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
				else if (left.Kill == right.Kill && left.Idx < right.Idx)
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

	
	GameRule(const char* name);
	void Awake()override;
	void Update()override;
	//ゲームルール設定
	//第二引数はそれぞれのルールに対応したもの（ストック数,制限時間(分),KO数）
	void SetGameRule(const GameRuleE& rule,const int& value);
	//ルール取得
	const GameRule::GameRuleE& GetGameRule();
	//値取得
	const int& GetValue();
	//ストック数取得
	const int& GetStock();
	//残り時間取得
	int GetRemainingTime();
	//試合が終了したかチェック
	const GameStateE& IsGameSet();
	//試合のステート取得
	const GameStateE& GetGameState();

	//プレイヤーセット
	void SetPlayer(Player* p,const int& index);
	//プレイヤーのスコア更新
	void UpdateScore(const int& index);
	//プレイヤー完全に死んだ通知を受け取る
	void PlayerDeath(const int& index);
	//ランキング取得
	const list<Ranking>& GetRanking();
private:
	//ランキング更新
	void _UpdateRanking();
	//ストック制のゲーム終了チェック
	bool _StockGameSet();
	//時間制限制のゲーム終了チェック
	bool _TimeLimitGameSet();
	//ノックアウト制のゲーム終了チェック
	bool _KnockOutGameSet();
private:
	//プレイヤー達を管理？
	Player* _Players[PLAYER_NUM];
	//何人ﾌﾟﾚｲしているか？
	int _PlayerCount;
	//何人死んだか？
	int _DeathCount;
	//順位
	list<Ranking> _Ranking;

	//ゲームのステート
	GameStateE _GameState;
	//ゲームルール
	GameRuleE _GameRule;
	//ストック、制限時間、KO数のいずれかが入る。
	int _Value;
	//時間を計る用の変数
	float _Timer;


	//関数ポインタ
	//GameRuleクラスの関数ポインタを受け取るようにする。
	bool(GameRule::*CheckGameSet)();
};