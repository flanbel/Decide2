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
		Blow			//吹き飛び
	};
	Player(char* name):GameObject(name)
	{
		
	}
	void Awake()override;
	void Start()override;
	void Update()override;
	void LateUpdate()override;
	void Render()override;

	void Move();
	void Attack();
	void Blown();
	void ItemAction();

	void SetIdx(int idx)
	{
		playeridx = idx;
	}
	int GetIdx()
	{
		return playeridx;
	}
	void SetRankList(list<Player*>* list)
	{
		rankList = list;
	}
	void SetColor(Color c)
	{
		playerColor = c;
	}
	//キャラクターの情報受け取り
	void SetInfo(CharacterInfo* info);
private:
	SkinModel* model;
	Animation* anim;
	RigidBody* rigid;
	btRigidBody* rb;
	PlayerParameter* Pparameter;
	CharacterParam Cparameter;
	D3DXFRAME_DERIVED* handFrame;
	Item* haveItem;
	//蓄積しているダメージ
	int damage;
	//残機
	int stock;
	//プレイヤーの添え字0~3
	int playeridx;
	//ジャンプ中か否か
	bool jump;
	//アクション(攻撃や防御)中か否か
	bool isAction;
	//チャージ率
	double CharagePower;
	//ステート
	PlayerState state, old;
	//表示用プレート
	PlatePrimitive* idxPlate;
	//色
	Color playerColor;
	//パーティクル
	ParticleEmitter *smoke;
	
	//吹き飛び
	Vector3 blown;
	//硬直
	float rigor;
	//加算用
	float time;

	//苦肉
	list<Player*>* rankList;
	//各効果音
	SoundSource *PunchSound;
};