#include "ResultScene.h"
#include "fbEngine/TextObject.h"
#include "fbEngine/SoundSource.h"
#include "fbEngine/ImageObject.h"
#include "fbEngine/Sprite.h"

void ResultScene::Start()
{
	//ResultBack
	ImageObject* back = GameObjectManager::AddNew<ImageObject>("back", 0);
	back->SetTexture(LOADTEXTURE("ResultBack.jpg"));
	back->transform->SetLocalPosition(Vector3(WindowW / 2, WindowH / 2, 0));

	ImageObject* result = GameObjectManager::AddNew<ImageObject>("result", 1);
	result->SetPivot(0.5f, 0.0f);
	result->SetTexture(LOADTEXTURE("Result.png"));
	result->transform->SetLocalPosition(Vector3(WindowW / 2-50, 10, 0));

	//音の再生
	SoundSource *sound = GameObjectManager::AddNew<SoundSource>("test", 1);
	sound->Init("Asset/Sound/Result.wav");
	sound->Play(false);
	//ゲームルール取得
	_GameRule = (GameRule*)GameObjectManager::FindObject("GameRule");
	//ランキング作成
	_CreateRanking(_GameRule->GetRanking());
}

void ResultScene::Update()
{
	//誰かがスタートボタンを押したら
	bool start = INSTANCE(InputManager)->IsPushButtonAll(XINPUT_GAMEPAD_START);

	if(KeyBoardInput->isPush(DIK_RETURN) || start && !_ChangeScene)
	{
		_ChangeScene = true;
		SetFade(true);
	}

	if(_ChangeScene && !_IsFade)
	{
		//シーン切り替え
		INSTANCE(SceneManager)->ChangeScene("CharaSelectScene");
		return;
	}
}

void ResultScene::_CreateRanking(const list<GameRule::Ranking>& ranking)
{
	int idx = 0;
	//一つ前のやつのを保存
	GameRule::Ranking beforeRank = {-1,-1,-1,-1};
	//1位から最後まで
	for each (GameRule::Ranking rank in ranking)
	{
		TextObject* rtext = GameObjectManager::AddNew<TextObject>("RankingText", 0);
		float space = 150.0f;
		rtext->transform->SetLocalPosition(Vector3(WindowW/2, 200.0f+(space * idx), 0.0f));
		rtext->Initialize(L"", 80.0f, Color::white, fbSprite::SpriteEffectE::SHADOW, "HGS明朝E");
		//文字作成
		wchar_t display[128];
		//順位
		wchar_t junni[2] = { 48 + (rank.Rank),0 };
		wcscpy(display, junni);
		wcscat_s(display, 128, L"位Player");
		//添え字を文字に
		wchar_t idxW[2] = { 48 + (rank.Idx + 1),0 };
		wcscat_s(display, 128, idxW);
		wcscat_s(display, 128, L"     KILL:");
		//倒した数
		wchar_t killW[4];
		Support::ToString(rank.Kill, killW);
		wcscat_s(display, 128, killW);
		//テキストセット
		rtext->SetString(display);

		//順位を保存しておく
		beforeRank = rank;
		idx++;
	}
}