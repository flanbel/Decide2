#pragma once
#include "fbEngine/Scene.h"

struct CharacterParam;
class CharaSelect;
class CharaRingFrame;
class ImageObject;
class GameRule;
class TextObject;

class CharaSelectScene : public Scene
{
public:
	CharaSelectScene() {};
	void Start()override;
	void Update()override;
private:
	//キャラクターフレーム作成
	void _CreateCharaFrame(const wchar_t **nameArray,const char** pathArray,const CharacterParam *paramArray);
	//ゲームルール更新
	void _UpdateGameRule();
private:
	//下のセレクトのやつ
	CharaSelect* _SelectArray[PLAYER_NUM];
	//各キャラクターのリング
	CharaRingFrame* _RingArray[CHARACTER_NUM];
	//画像
	ImageObject* _ReadyFight;
	//ゲームルール
	GameRule* _GameRule;
	//ゲームルールを表示するテキスト
	TextObject* _DisplayGameRule;
};