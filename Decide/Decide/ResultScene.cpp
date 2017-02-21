#include "ResultScene.h"
#include "fbEngine/TextObject.h"
#include "fbEngine/SoundSource.h"
#include "fbEngine/ImageObject.h"

void ResultScene::Start()
{
	//ResultBack
	ImageObject* back = GameObjectManager::AddNew<ImageObject>("back", 0);
	TEXTURE* backT = TextureManager::LoadTexture("ResultBack.jpg");
	backT->pivot = Vector2(0.5, 0.5);
	back->SetTexture(backT);
	back->transform->localPosition = Vector3(WindowW / 2, WindowH / 2, 0);

	ImageObject* result = GameObjectManager::AddNew<ImageObject>("result", 1);
	TEXTURE* tex = TextureManager::LoadTexture("Result.png");
	tex->pivot = Vector2(0.5, 0.0);
	result->SetTexture(tex);
	result->transform->localPosition = Vector3(WindowW / 2-50, 10, 0);

	//音の再生
	SoundSource *sound = GameObjectManager::AddNew<SoundSource>("test", 1);
	sound->Init("Asset/Sound/Result.wav");
	sound->Play(false);
	//ゲームルール取得
	gamerule = (GameRule*)GameObjectManager::FindObject("GameRule");
	//ランキング作成
	CreateRanking(gamerule->GetRank());
}

void ResultScene::Update()
{
	bool flag = false;
	FOR(PLAYER_NUM)
	{
		//誰かがスタートボタンを押したら
		if (XboxInput(i)->isPushButton(XINPUT_GAMEPAD_START))
		{
			flag = true;
			break;
		}
	}
	if(KeyBoardInput->isPush(DIK_RETURN) || flag)
	{
		//シーン切り替え
		INSTANCE(SceneManager)->ChangeScene("CharaSelectScene");
		return;
	}
}

void ResultScene::CreateRanking(list<GameRule::Ranking> ranking)
{
	int idx = 0;
	//一つ前のやつのを保存
	GameRule::Ranking beforeRank;
	//1位から最後まで
	for each (GameRule::Ranking rank in ranking)
	{
		TextObject* rt = GameObjectManager::AddNew<TextObject>("rt", 0);
		rt->transform->localPosition = Vector3(300, 150 * idx + 150, 0);
		rt->SetFontStyle("HGS明朝E");
		rt->SetBlendColor(Color::white);
		rt->SetFontSize(80);
		//文字作成
		wchar_t display[128];
		//順位
		int r = idx;
		//ストック以外は順位が被る場合がある。
		if(gamerule->GetGameRule() != GameRule::GAMERULE::STOCK)
		{
			//倒した数が同じなら
			if (beforeRank.Kill == rank.Kill)
				//順位を同じに
				r = beforeRank.idx;
		}
		wchar_t junni[2] = { 48 + (r + 1),0 };
		wcscpy_s(display, 2, junni);
		wcscat_s(display, 128, L"位Player");
		//添え字を文字に
		wchar_t idxW[2] = { 48 + (rank.idx + 1),0 };
		wcscat_s(display, 128, idxW);
		wcscat_s(display, 128, L"     KILL:");
		wchar_t killW[2] = { 48 + rank.Kill,0 };
		wcscat_s(display, 128, killW);
		rt->SetString(display);

		idx++;
		//順位を保存しておく
		beforeRank.idx = r;
		beforeRank.Kill = rank.Kill;
	}
}