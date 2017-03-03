#include "BattleScene.h"

#include "GameCamera.h"
#include "GameLight.h"
#include "GameShadowCamera.h"
#include "Player.h"
#include "Stage.h"
#include "Sky.h"
#include "fbEngine/ImageObject.h"
#include "fbEngine/TextObject.h"
#include "fbEngine/Sprite.h"
#include "fbEngine/SoundSource.h"
#include "Item.h"
#include "GameRule.h"
void BattleScene::Start()
{
	GameObjectManager::AddNew<GameCamera>("GameCamera", 0);
	GameObjectManager::AddNew<GameLight>("GameLight", 0);
	GameObjectManager::AddNew<GameShadowCamera>("GameShadowCamera", 0);

	GameObjectManager::AddNew<Sky>("Sky", 0);
	GameObjectManager::AddNew<Stage>("Stage", 1);
	gong = GameObjectManager::AddNew<SoundSource>("Gong", 0);
	BGM = GameObjectManager::AddNew<SoundSource>("BGM", 0);
	gong->Init("Asset/Sound/gong.wav");
	BGM->Init("Asset/Sound/bgm_Battle.wav");
	BGM->Play(true);
	//シーン切り替えフラグ
	Change = false;

	_GameRule = (GameRule*)GameObjectManager::FindObject("GameRule");
	_GameRule->Discard(false);
	if(_GameRule->GetGameRule() == GameRule::GAMERULE::TIMELIMIT)
	{
		//制限時間表示用のアレ
		Timer = GameObjectManager::AddNew<TextObject>("Timer", 1);
		Timer->Initialize(L"", 80.0f, Color::white, true, "HGS明朝E");
		Timer->transform->localPosition = Vector3(WindowW/2, 80, 0);
		wchar_t time[10];
		InttoString(_GameRule->GetRemainingTime(), time);
		Timer->SetString(time);
		lastTime = -1;
	}
	else
		Timer = nullptr;

	//深度の画像を書く
	test = GameObjectManager::AddNew<ImageObject>("test", 4);
	test->SetTexture(INSTANCE(RenderTargetManager)->GetRenderTargetTexture(0));
	test->SetActive(false);
}

#include "ResultScene.h"
void BattleScene::Update()
{
	BGM->Update();
	if (KeyBoardInput->isPush(DIK_C))
	{
		Item* i = GameObjectManager::AddNew<Item>("item", 1);
		i->transform->localPosition = Vector3(0, 200, 0);
		//test->SetActive(!test->GetActive());
	}

	int nowtime = _GameRule->GetRemainingTime();
	if(Timer != nullptr &&
		lastTime != nowtime)
	{
		wchar_t time[10];
		InttoString(nowtime, time);
		Timer->SetString(time);
		lastTime = nowtime;
	}

	//試合が終了したなら。
	if (_GameRule->IsGameSet() && !Change)
	{
		//音再生
		gong->Play(false);
		Change = true;
	}
	//切り替え
	if(!gong->IsPlaying() && Change)
	{
		//バイブレーションを止める
		FOR(PLAYER_NUM)
			XboxInput(i)->Vibration(0, 0);
		_GameRule->CreateScore();
		INSTANCE(SceneManager)->ChangeScene("ResultScene");
		return;
	}
}