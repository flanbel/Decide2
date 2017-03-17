#pragma once
#include "fbEngine/GameObject.h"
#include "Character.h"
//#include "PlayerState.h"
//#include "PlayerState_Stay.h"
//#include "PlayerState_Walk.h"
//#include "PlayerState_Run.h"
//#include "PlayerState_Jump.h"
//#include "PlayerState_Fall.h"
//#include "PlayerState_Punch.h"
//#include "PlayerState_Kick.h"
//#include "PlayerState_KickCharge.h"
//#include "PlayerState_Blow.h"

class SkinModel;
class Animation;
class PlayerParameter;
class SoundSource;;
class PlatePrimitive;
class ParticleEmitter;
struct D3DXFRAME_DERIVED;
class Item;
class GameRule;
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
	void LateUpdate()override;

	void UpdateStateMachine();	//ステートマシン更新
	void ChangeState(PStateE s);		//ステート切り替え
	void PlayAnimation(int idx, float time, int loop = -1);
	void AnimationControl();		//ステートによって再生するアニメーションを指定
	void Attack();
	void Move();
	void Jump();
	void Damage();
	void Blown();
	void ItemAction();
	//落ちた時の処理
	void Death();

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
	//一位かどうか？
	void No1(bool f);

	const Vector3 GetDir();
private:
	//初期化
	void _Reset();
	//重力処理
	void _GravityCheck(const float& movey);
	//PlayerState* currentState = nullptr;	//現在のステート.
	//PlayerStateStay StayState;				//待機ステート.
	//PlayerStateWalk WalkState;				//歩きステート.
	//PlayerStateRun RunState;				//走りステート.
	//PlayerStateJump JumpState;
	//PlayerStateFall FallState;
private:
	//コンポーネントとかアドレスの保持が必要なものたち
	SkinModel* _Model;
	Animation* _Anim;
	RigidBody* _Rigid;
	btRigidBody* _RB;
	PlayerParameter* _Pparameter;
	const CharacterParam* _Cparameter;
	D3DXFRAME_DERIVED* _HandFrame;
	Item* _HaveItem;
	//リジットボディの高さ
	float _Height;
	//ゲームルール
	GameRule* _GameRule;
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
	bool _Jump;
	//ジャンプ数
	int _JumpCount;
	//チャージ率
	double _CharagePower;
	//ステート
	PStateE _State;
	//表示用プレート
	PlatePrimitive *_IdxPlate, *_CrownPlate;
	//プレイヤーの色
	Color _PlayerColor;
	//パーティクル
	ParticleEmitter *_Smoke, *_Fire;

	//最終的な移動量
	Vector3 _Move;
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

	//各効果音　
	SoundSource *_PunchSound;
};