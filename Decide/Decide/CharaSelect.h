#pragma once
#include "fbEngine/GameObject.h"
#include "Character.h"

class ImageObject;
class TextObject;
class ShowModel;
struct RenderTarget;
class SoundSource;

class CharaSelect:public GameObject
{
public:
	CharaSelect(char* name) :GameObject(name) {}
	void Awake()override;
	void Start()override;
	void Update()override;
	void SetIdx(int idx)
	{
		playerIdx = idx;
	}
	void SetColor(Color c)
	{
		playerColor = c;
	}
	Color GetColor()
	{
		return playerColor;
	}
	void SetDecision(bool d)
	{
		decision = d;
	}
	bool GetDecision()
	{
		return decision;
	}
	Vector2 GetCursorPos();
	//キャラクター情報セット
	void SetInfo(CharacterInfo* i);
	//
	CharacterInfo* GetInfo();
	void Release();
private:
	ImageObject* cursor,*back,*show;
	TextObject* name;
	//プレイヤーの添え字
	int playerIdx;
	//プレイヤーの色
	Color playerColor;
	//決定しているかどうか
	bool decision;
	//情報
	CharacterInfo* info;
	CharacterInfo* old;
	ShowModel* showmodel;
	RenderTarget* render;
	SoundSource *sound;
};