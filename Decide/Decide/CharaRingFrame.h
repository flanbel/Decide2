#pragma once
#include "fbEngine/_Object\_GameObject/GameObject.h"
#include "Character.h"
class ImageObject;
class CircleCollision;
//キャラクター選択時のリングアイコン
class CharaRingFrame :public GameObject
{
public:
	CharaRingFrame(const char* name) :GameObject(name) {}
	~CharaRingFrame()
	{
		FOR(idx, PLAYER_NUM)
		{
			SAFE_DELETE(info[idx]);
		}
	}
	void Awake()override;
	void Start()override;
	void Update()override;
	bool Judgment(Vector2 pos);
	void SetInfo(CharacterInfo* i,int idx)
	{
		info[idx] = i;
	}
	CharacterInfo* GetInfo(int idx)
	{
		return info[idx];
	}
private:
	ImageObject* ring;
	CircleCollision* circle;
	CharacterInfo* info[PLAYER_NUM];
};