#pragma once
#include "fbEngine/_Object\_GameObject/GameObject.h"

namespace fbEngine
{
	enum ItemTypeE
	{
		Throw,	//投げるタイプ
		Slash,	//振るタイプ
	};
}

namespace
{
	//アイテムのデータ
	struct ItemData
	{
		char* modelname;		//モデルのパス
		Vector3 collisionsize;	//当たり判定の大きさ
		Vector3 offset;			//当たり判定の移動量
		int actiontype;			//投げた時に使う関数のタイプ
		int itemtype;			//アイテムのタイプ
	};
}

class Item:public GameObject
{
public:
	Item(const char* name);
	~Item();
	void Awake()override;
	void Update()override;
	//アイテムを持つときに呼び出す関数
	//引数はキャラクターの持ち手のボーンの行列。
	//持てたならtrue,持てないならfalse
	bool ToHave(const D3DXMATRIX* handmat);
	//アイテムを離す
	//引数は投げ飛ばすベクトル
	void ToSeparate(const Vector3& vec = Vector3::zero,int idx = -1);

	const int& GetItemType()
	{
		return _ItemType;
	}

	//フレンドにしたかった奴
	const int& GetOwner()
	{
		return _DropOwner;
	}
	const Vector3& GetMove()
	{
		return _Move;
	}
public:
	//フレンド関数たち
	//friend void Standard(Item* item, vector<Collision*> colls);
	//friend void Bom(Item* item, vector<Collision*> colls);
private:
	//投げられている処理
	void  _Thrown();
	//削除するかチェック
	void _DeleteCheck();
private:
	Animation* _Anim;	//アニメーション
	RigidBody* _Rigid;	//剛体(剛体である必要はあるか？)	

	int _ItemID;		//アイテムの種類を判別する一意な番号
	int _ItemType;		//アイテムのタイプ
	float _LifeTime;	//寿命
	float _Timer;		//タイマー
	Vector3 _Move;		//移動量
	int _DropOwner;			//落とし主

	bool _IsHave;//所持されている。
	//持ち手(handle)の行列(アイテム側)
	D3DXMATRIX* _ItemHandleMat;
	//持つボーンの行列(キャラクター側)
	const D3DXMATRIX* _HandMat;

	//各アイテムのアクション
	void(*_ItemAction)(Item* item, vector<Collision*> colls);
};