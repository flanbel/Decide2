#pragma once
#include "fbEngine/GameObject.h"
class SkinModelData;
class Animation;
//キャラクターのパラメーター
struct CharacterParam
{
public:
	CharacterParam()
	{
		speed = 0;
		power = 0;
		blowCorrection = 0;
		toBlowCorrection = 0;
	};
	CharacterParam(float s, float p, float b, float tob)
	{
		speed = s;
		power = p;
		blowCorrection = b;
		toBlowCorrection = tob;
	}
	//速度
	float speed;
	//攻撃力
	float power;
	//吹き飛ばし補正()
	float blowCorrection;
	//吹き飛び補正
	float toBlowCorrection;
};

//キャラクターの情報
struct CharacterInfo:Noncopyable
{
public:
	CharacterInfo(wchar_t* n,char* p, CharacterParam par,SkinModelData* d, Animation* a = nullptr)
	{
		name = n;
		path = p;
		data = d;
		anim = a;
		parameter = par;
	}
	~CharacterInfo();
	
	//名前
	wchar_t* name;
	//データパス
	char* path;
	//モデルデータ
	SkinModelData* data;
	//アニメーション
	Animation* anim;
	//パラメータ
	CharacterParam parameter;
};
//キャラクタークラス
class Character:public GameObject
{
public:
	void Start()override;
};