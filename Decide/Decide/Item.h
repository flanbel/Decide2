#pragma once
#include "fbEngine/GameObject.h"

class Animation;

//アイテムのデータ
struct ItemData
{
	char* modelname;
	Vector3 collisionsize;
};

class Item:public GameObject
{
public:
	Item(const char* name);
	void Awake()override;
	void Update()override;
	void LateUpdate()override;
	//アイテムを持つときに呼び出す関数
	//引数はキャラクターの持ち手のボーンの行列。
	//持てたならtrue,持てないならfalse
	bool ToHave(const D3DXMATRIX* handmat);
	//アイテムを離す
	//引数は投げ飛ばすベクトル
	void ToSeparate(const Vector3& vec = Vector3::zero,int idx = -1);
private:
	Animation* _Anim;	//アニメーション
	RigidBody* _Rigid;	//剛体(剛体である必要はあるか？)	
	float _LifeTime;	//寿命
	float _Timer;		//タイマー
	Vector3 _Move;	//移動量

	bool _IsHave;//所持されている。
	//持ち手(handle)の行列(アイテム側)
	D3DXMATRIX* _ItemHandleMat;
	//持つボーンの行列(キャラクター側)
	const D3DXMATRIX* _HandMat;
};