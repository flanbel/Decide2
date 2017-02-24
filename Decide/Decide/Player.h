#pragma once
#include "fbEngine/GameObject.h"
#include "Character.h"

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
	enum PlayerState
	{
		Stay = 0,		//待機
		Walk,			//歩き
		Dash,			//ダッシュ
		Punch,			//パンチ
		Kick_Charge,	//蹴りため
		Kick_Shot,		//蹴り
		Damage,			//ダメージ
		Blow,			//吹き飛び
		USESWORD,		//剣振り
		ISJUMP,			//ジャンプ中
	};
	Player(char* name);
	void Awake()override;
	void Start()override;
	void Update()override;
	void LateUpdate()override;
	void Render()override;

	void Move();
	void Attack();
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
private:
	SkinModel* model;
	Animation* anim;
	RigidBody* rigid;
	btRigidBody* rb;
	PlayerParameter* Pparameter;
	CharacterParam Cparameter;
	D3DXFRAME_DERIVED* handFrame;
	Item* haveItem;
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
	//ジャンプ中か否か
	bool jump;
	//落ちている(降下中)
	bool fall;
	int jumpCount;
	//アクション(攻撃や防御)中か否か
	bool isAction;
	//攻撃が当たった時に止まる
	bool hitStop;
	//止まる時間
	int stopTime;
	//カウント
	int hitStopCount;
	//チャージ率
	double CharagePower;
	//ステート
	PlayerState state, old;
	//表示用プレート
	PlatePrimitive* idxPlate;
	//プレイヤーの色
	Color playerColor;
	//パーティクル
	ParticleEmitter *smoke;
	
	//吹き飛び
	Vector3 blown;
	//硬直
	float rigor;
	//加算用
	float time;

	//各効果音　
	SoundSource *PunchSound;
};