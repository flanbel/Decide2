#include "CharaSelectScene.h"
#include "fbEngine/ImageObject.h"
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

	ImageObject* backGround = GameObjectManager::AddNew<ImageObject>("Back",0);
	backGround->SetTexture(TextureManager::LoadTexture("SelectBack.png"));

	readyFight = GameObjectManager::AddNew<ImageObject>("ReadyToFight", 1);
	readyFight->SetTexture(TextureManager::LoadTexture("ReadyToStart.png"));
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

	//決定しているプレイヤー数カウント
	int count = 0;
	FOR(4)
	{
		if (selectArray[i]->GetDecision())
			count++;
	}
	//一人以上選択されている
	if (count > 1)
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
			BattleScene* battle = (BattleScene*)INSTANCE(SceneManager)->ChangeScene("BattleScene");
			battle->SetPlayer(Array);
			return;
		}
	}else
	{
		readyFight->SetActive(false);
	}
}
