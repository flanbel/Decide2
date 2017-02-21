#include "BattleScene.h"

#include "GameCamera.h"
#include "GameLight.h"
#include "GameShadowCamera.h"
#include "Player.h"
#include "Stage.h"
#include "Sky.h"
#include "fbEngine/ImageObject.h"
#include "fbEngine/Sprite.h"
#include "fbEngine/SoundSource.h"
#include "Item.h"
#include "GameRule.h"
void BattleScene::Start()
{
	GameObjectManager::AddNew<GameCamera>("GameCamera", 0);
	GameObjectManager::AddNew<GameLight>("GameLight", 0);
	GameObjectManager::AddNew<GameShadowCamera>("GameShadowCamera", 0);

	GameObjectManager::AddNew<Stage>("Stage", 0);
	GameObjectManager::AddNew<Sky>("Sky", 0);
	gong = GameObjectManager::AddNew<SoundSource>("Gong", 0);
	BGM = GameObjectManager::AddNew<SoundSource>("BGM", 0);
	gong->Init("Asset/Sound/gong.wav");
	BGM->Init("Asset/Sound/NGAHHH!!.wav");
	BGM->Play(true);
	//シーン切り替えフラグ
	Change = false;

	gamerule = (GameRule*)GameObjectManager::FindObject("GameRule");
	gamerule->Discard(false);

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

	//試合終了判定
	Player* first = nullptr;
	int count = 0;
	FOR(playerNum)
	{
		if(playerArray[i]->GetActive())
		{
			count++;
			first = playerArray[i];
		}
	}
	//最後に残ったのが一人なら
	if ((count <= 1 && !Change))
	{
		//音再生
		gong->Play(false);
		Change = true;
	}
	//切り替え
	if(!gong->IsPlaying() && Change)
	{
		vector<int> rank;
		if (first != nullptr)
			rank.push_back(first->GetIdx());
		list<Player*>::iterator it = rankList.begin();
		while (it != rankList.end())
		{
			rank.push_back((*it)->GetIdx());
			it++;
		}
		rankList.clear();
		ResultScene* result = (ResultScene*)INSTANCE(SceneManager)->ChangeScene("ResultScene");
		result->Setrank(rank);
		return;
	}
}

void BattleScene::LateUpdate()
{

}

void BattleScene::Render()
{
	
}

void BattleScene::SetPlayer(vector<Player*> players)
{
	playerNum = players.size();
	FOR(playerNum)
	{
		//アドレスを渡す
		playerArray[i] = players[i];
		playerArray[i]->SetRankList(&rankList);
	}
}
