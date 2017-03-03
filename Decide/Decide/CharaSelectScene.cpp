#include "CharaSelectScene.h"
#include "fbEngine/ImageObject.h"
#include "fbEngine/TextObject.h"
#include "CharaSelect.h"
#include "CharaRingFrame.h"
#include "Character.h"
#include "fbEngine/SkinModelData.h"
#include "fbEngine/Animation.h"
#include "SelectCamera.h"
#include "GameLight.h"
#include "GameShadowCamera.h"
#include "Player.h"
#include "BattleScene.h"
#include "GameRule.h"

#include "fbEngine/CircleCollision.h"
#include "fbEngine/Sprite.h"
void CharaSelectScene::Start()
{
	GameObjectManager::AddNew<SelectCamera>("camera", 0);
	GameObjectManager::AddNew<GameLight>("GameLight", 0);
	GameObjectManager::AddNew<GameShadowCamera>("GameShadowCamera", 0);

	gameRule = GameObjectManager::AddNew<GameRule>("GameRule", 0);
	gameRule->Discard(false);
	displayGameRule = GameObjectManager::AddNew<TextObject>("DisplayGameRule", 1);
	displayGameRule->Initialize(L"GameRule:TimeLimit 2", 40.0f, Color::white, SpriteEffectE::SHADOW, "HGS明朝E");
	displayGameRule->transform->localPosition = Vector3(50, 100, 0);

	ImageObject* backGround = GameObjectManager::AddNew<ImageObject>("Back",0);
	backGround->SetTexture(LOADTEXTURE("SelectBack.png"));
	backGround->SetPivot(0.0f, 0.0f);

	readyFight = GameObjectManager::AddNew<ImageObject>("ReadyToFight", 1);
	readyFight->SetTexture(LOADTEXTURE("ReadyToStart.png"));
	readyFight->SetPivot(0.0f, 0.5f);
	readyFight->transform->localPosition = Vector3(0, 250, 0);
	readyFight->SetActive(false);
	wchar_t* name[] = {
		L"TV",
		L"" 
	};
	//\Asset/Xfile から先のパス
	char* path[] = {
		"TV/TVman1.X",
		"" 
	};
	//キャラクターのパラメーター
	CharacterParam Param[] =
	{
		{3.0f,2.0f,1.2f,1.0f},
	};
	//キャラクタ数分作成
	for(short j = 0;j<CHARACTER_NUM;j++)
	{
		ringArray[j] = GameObjectManager::AddNew<CharaRingFrame>("frame", 0);
		ringArray[j]->transform->localPosition = Vector3(WindowW / 2, 100, 0);
		//プレイヤ数分情報作成
		FOR(PLAYER_NUM)
		{
			//このへんメモリリーク注意
			Animation* anim = new Animation(nullptr, nullptr);
			SkinModelData* data = new SkinModelData();
			data->CloneModelData(SkinModelManager::LoadModel(path[j]), anim);
			CharacterInfo* info = new CharacterInfo(name[j], path[j], Param[j], data, anim);
			ringArray[j]->SetInfo(info,i);
		}
	}
	//プレイヤーのやつの色
	Color playercolor[PLAYER_NUM] =
	{
		Color::red,
		Color::blue,
		Color::yellow,
		Color::green
	};
	//選択のやつを生成
	FOR(PLAYER_NUM)
	{
		selectArray[i] = GameObjectManager::AddNew<CharaSelect>("Select", 0);
		selectArray[i]->SetIdx(i);
		selectArray[i]->SetColor(playercolor[i]);
		selectArray[i]->transform->localPosition = Vector3(i*290+70, 270, 0);
	}
}

void CharaSelectScene::Update()
{
	//キャラクター選択
	FOR(PLAYER_NUM)
	{
		//jはキャラクター数
		short j = 0;
		for(;j<CHARACTER_NUM;j++)
		{
			//各カーソルと重なっているか判定
			if(ringArray[j]->Judgment(selectArray[i]->GetCursorPos()))
			{
				//リングの情報セット
				selectArray[i]->SetInfo(ringArray[j]->GetInfo(i));
				
				break;
			}
		}
		//何もセットしない
		if(j == CHARACTER_NUM && selectArray[i]->GetDecision() == false)
		{
			selectArray[i]->SetInfo(nullptr);
		}
	}
	//現在のルール取得
	GameRule::GAMERULE rule = gameRule->GetGameRule();
	int value = gameRule->GetValue();
	bool change = false;
	//ゲームルール選択
	if(KeyBoardInput->isPush(DIK_UP))
	{
		rule = (GameRule::GAMERULE)((rule + 1) % GameRule::GAMERULE::NUM);
		value = 3;
		change = true;
	}
	if (KeyBoardInput->isPush(DIK_DOWN))
	{
		rule = (GameRule::GAMERULE)((rule - 1) < 0 ? (GameRule::GAMERULE::NUM -1) : (rule - 1));
		value = 3;
		change = true;
	}
	if (KeyBoardInput->isPush(DIK_RIGHT))
	{
		value = min(value + 1, 99);
		change = true;
	}
	if (KeyBoardInput->isPush(DIK_LEFT))
	{
		value = max(value - 1, 1);
		change = true;
	}
	//変更されているなら
	if (change)
	{
		gameRule->SetGameRule(rule, value);
		
		wchar_t display[128];
		wchar_t *ruleW = NULL;
		wchar_t valueW[5] = {L'0'};
		switch (rule)
		{
		case GameRule::STOCK:
			ruleW = L"Stock ";
			break;
		case GameRule::TIMELIMIT:
			ruleW = L"TimeLimit ";
			break;
		case GameRule::KNOCKOUT:
			ruleW = L"KnockOut ";
			break;
		default:
			break;
		}
		InttoString(value, valueW);
		//文字列コピー
		wcscpy_s(display, wcslen(L"GameRule:")+1, L"GameRule:");
		//文字列追加
		wcscat_s(display, wcslen(display) + wcslen(ruleW)+1, ruleW);
		wcscat_s(display, wcslen(display) + wcslen(valueW)+1, valueW);
		//表示更新
		displayGameRule->SetString(display);
	}
	

	//決定しているプレイヤー数カウント
	int count = 0;
	FOR(4)
	{
		if (selectArray[i]->GetDecision())
			count++;
	}
	//二人以上選択されている
	if (count >= 2)
	{
		bool flag = false;
		FOR(PLAYER_NUM)
		{
			if (XboxInput(i)->isPushButton(XINPUT_GAMEPAD_START))
			{
				flag = true;
				break;
			}
		}

		//ReadyToFight表示
		readyFight->SetActive(true);
		//戦闘シーンへ移行
		if (KeyBoardInput->isPush(DIK_RETURN) || flag)
		{
			vector<Player*> Array;
			FOR(PLAYER_NUM)
			{
				//選択されているなら
				if (selectArray[i]->GetDecision())
				{
					//プレイヤー生成
					Player* p = GameObjectManager::AddNew<Player>("Player", 1);
					p->SetIdx(i);
					p->SetInfo(selectArray[i]->GetInfo());
					p->SetColor(selectArray[i]->GetColor());
					//シーン切り替えしても破棄しないように設定
					p->Discard(false);
					Array.push_back(p);
					//ゲームルールの方にプレイヤーの情報をセット
					gameRule->SetPlayer(p, i);
				}
			}
			//選択のやつ解放
			FOR(PLAYER_NUM)
			{
				selectArray[i]->Release();
			}
			//切り替え
			INSTANCE(SceneManager)->ChangeScene("BattleScene");
			return;
		}
	}else
	{
		readyFight->SetActive(false);
	}
}
