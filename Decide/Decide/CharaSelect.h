#pragma once
#include "fbEngine/GameObject.h"
#include "Character.h"

class ImageObject;
class TextObject;
class ShowModel;
class SkinModel;
class Animation;
struct RenderTarget;
class SoundSource;

class CharaSelect:public GameObject
{
public:
	CharaSelect(const char* name) :GameObject(name) {}
	void Awake()override;
	void Start()override;
	void Update()override;

	void SetIdx(int idx)
	{
		_PlayerIdx = idx;
	}
	void SetColor(const Color& c)
	{
		_PlayerColor = c;
	}
	const Color& GetColor()
	{
		return _PlayerColor;
	}
	void SetDecision(bool d)
	{
		_Decision = d;
	}
	bool GetDecision()
	{
		return _Decision;
	}
	Vector2 GetCursorPos();
	//キャラクター情報セット
	void SetInfo(CharacterInfo* i);
	//
	CharacterInfo* GetInfo();
	void Release();
private:
	ImageObject* _Cursor,*_Back,*_Show,*_OK;
	TextObject* _Name;
	//プレイヤーの添え字
	int _PlayerIdx;
	//プレイヤーの色
	Color _PlayerColor;
	//決定しているかどうか
	bool _Decision;
	//情報
	CharacterInfo* _Info;
	CharacterInfo* _Old;
	ShowModel* _ShowModel;
	Animation* _ShowAnim;
	SkinModel* _ShowSkinModel;
	RenderTarget* _Render;
	SoundSource *_Sound;
};