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
class Player:public GameObject
{
public:
	//プレイヤーのステート
	enum PState
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
	Player(char* name);
	void Awake()override;
	void Start()override;
	void Update()override;
	void LateUpdate()override;

	void UpdateStateMachine();	//ステートマシン更新
	void ChangeState(PState s);		//ステート切り替え
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
		playeridx = idx;
	}
	int GetIdx()
	{
		return playeridx;
	}
	void SetColor(Color c)
	{
		playerColor = c;
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
	//PlayerState* currentState = nullptr;	//現在のステート.
	//PlayerStateStay StayState;				//待機ステート.
	//PlayerStateWalk WalkState;				//歩きステート.
	//PlayerStateRun RunState;				//走りステート.
	//PlayerStateJump JumpState;
	//PlayerStateFall FallState;
private:
	//コンポーネントとかアドレスの保持が必要なものたち
	SkinModel* model;
	Animation* anim;
	RigidBody* rigid;
	btRigidBody* rb;
	PlayerParameter* Pparameter;
	CharacterParam Cparameter;
	D3DXFRAME_DERIVED* handFrame;
	Item* haveItem;
	//ゲームルール
	GameRule* gameRule;
	//蓄積しているダメージ
	int damage;
	//残機
	int stock;
	//倒した数
	int killCount;
	//(死んだとき)最期に攻撃を与えてきたPlayerの添え字
	int lastAttack;
	//プレイヤーの添え字0~3
	int playeridx;
	//アクション中かどうか(攻撃とか)
	bool isAction;
	//ジャンプ中か否か
	bool jump;
	//ジャンプ数
	int jumpCount;
	//チャージ率
	double CharagePower;
	//ステート
	PState state;
	//表示用プレート
	PlatePrimitive *idxPlate, *CrownPlate;
	//プレイヤーの色
	Color playerColor;
	//パーティクル
	ParticleEmitter *smoke,*fire;
	
	//最終的な移動量
	Vector3 move;
	//進行
	Vector3 dir;
	//吹き飛び
	Vector3 blown;
	//硬直
	float rigor;
	//硬直のタイマー
	float rigortimer;

	//各効果音　
	SoundSource *PunchSound;
};