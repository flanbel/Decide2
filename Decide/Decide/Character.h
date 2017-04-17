#pragma once
#include "fbEngine/_Object\_GameObject/GameObject.h"

//キャラクターのパラメーター
struct CharacterParam
{
public:
	CharacterParam()
	{
		Speed = 0;
		Power = 0;
		BlowCorrection = 0;
		ToBlowCorrection = 0;
	};
	CharacterParam(const float& s, const float& p, const float& b, const float& tob)
	{
		Speed = s;
		Power = p;
		BlowCorrection = b;
		ToBlowCorrection = tob;
	}
	//速度
	float Speed;
	//攻撃力
	float Power;
	//吹き飛ばし補正()
	float BlowCorrection;
	//吹き飛び補正
	float ToBlowCorrection;
};

//キャラクターの情報
struct CharacterInfo:Noncopyable
{
public:
	CharacterInfo(const wchar_t* n,const char* p,const CharacterParam* par,SkinModelData* d,Animation* a = nullptr)
	{
		Name = n;
		Path = p;
		Data = d;
		Anim = a;
		Parameter = par;
	}
	~CharacterInfo();
	
	//名前
	const wchar_t* Name;
	//データパス
	const char* Path;
	//モデルデータ
	SkinModelData* Data;
	//アニメーション
	Animation* Anim;
	//パラメータ
	const CharacterParam* Parameter;
};
//キャラクタークラス
class Character:public GameObject
{
public:
	void Start()override;
};