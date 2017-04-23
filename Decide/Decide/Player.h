#pragma once
#include "fbEngine/_Object\_GameObject/GameObject.h"
#include "Character.h"

class SoundSource;

class PlayerParameter;
class Plate;
class AnimationPlate;
class ParticleEmitter;
class Item;
class GameRule;
class DamageCollision;

//プレイヤークラス
class Player :public GameObject
{
public:
	//プレイヤーのステート
	enum PStateE
	{
		STAY,			//待機
		WALK,			//歩き
		DASH,			//ダッシュ
		JUMP,			//ジャンプ中
		FALL,			//落下中
		PUNCH,			//パンチ
		KICK_CHARGE,	//蹴りため
		KICK,			//蹴り
		SLASH,			//剣振り
		DAMAGE,			//ダメージ
		BLOW,			//吹き飛び
	};
	Player(const char* name);
	void Awake()override;
	void Start()override;
	void Update()override;
		
	//外部からダメージを加える
	void Damage(const int& idx,const float& damage,const Vector3& blow,const float& rigor);
	//ステート切り替え
	void ChangeState(PStateE s);

	
	void SetIdx(int idx)
	{
		_Playeridx = idx;
	}
	int GetIdx()
	{
		return _Playeridx;
	}
	void SetColor(Color c)
	{
		_PlayerColor = c;
	}
	//キャラクターの情報受け取り
	void SetInfo(CharacterInfo* info);
	void AddKillCount();
	int GetKillCount();
	int GetStockCount();
	bool GetAlive()
	{
		return _IsAlive;
	}
	//一位かどうか？
	void No1(bool f);

	const Vector3 GetDir();
private:
	//入力値チェック
	void CheckInput();
	//ステートによって再生するアニメーションを指定
	void _AnimationControl();
	//攻撃
	void _Attack();
	//アイテム関連
	void _ItemAction();
	//移動
	void _Move();
	//ジャンプ
	void _Jump();
	//吹き飛ばす
	void _BlowOff();
	//落ちた時の処理
	void _Death();
	//重力処理
	void _GravityCheck(const float& movey);
	//アニメーション再生。
	void _PlayAnimation(int idx, float time, int loop = -1);
	//初期化
	void _Reset();
private:
	//モデル
	SkinModel* _Model;
	//アニメーション
	Animation* _Anim;
	//剛体
	RigidBody* _Rigid;
	//剛体
	btRigidBody* _RB;
	//プレイヤーのダメージとか残機とか
	PlayerParameter* _Pparameter;
	//キャラクターの能力
	const CharacterParam* _Cparameter;
	//アイテムをもつフレーム
	D3DXFRAME_DERIVED* _HandFrame;
	//持っているアイテム
	Item* _HaveItem;
	//ゲームルール
	GameRule* _GameRule;

	//アニメーションの補完時間と遷移時間
	float _InterpolateTime, _TransitionTime;

	//リジットボディの高さ
	float _Height;
	//蓄積しているダメージ
	int _Damage;
	//残機
	int _Stock;
	//倒した数
	int _KillCount;
	//(死んだとき)最期に攻撃を与えてきたPlayerの添え字
	int _LastAttack;
	//プレイヤーの添え字0~3
	int _Playeridx;
	//アクション中かどうか(攻撃とか)
	bool _IsAction;
	//ジャンプ中か否か
	bool _isJump;
	//ジャンプ数
	int _JumpCount;
	//チャージ率
	double _CharagePower;
	//ステート
	PStateE _State;
	//表示用プレート
	Plate *_IdxPlate, *_CrownPlate;
	//エフェクト
	AnimationPlate *_Aura;
	//プレイヤーの色
	Color _PlayerColor;
	//パーティクル
	ParticleEmitter *_Smoke, *_Fire;

	//最終的な移動量
	Vector3 _Movement;
	//重力
	float _Gravity;
	//進行
	Vector3 _Dir;
	//吹き飛び
	Vector3 _Blown;
	//硬直
	float _Rigor;
	//硬直のタイマー
	float _RigorTimer;
	//生きている
	bool _IsAlive;
	//復活時間
	float _RespawnTimer;

	//各効果音　
	SoundSource *_DamageSE;
};