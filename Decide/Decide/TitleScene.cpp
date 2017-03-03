#include "TitleScene.h"
#include "fbEngine/ImageObject.h"
#include "fbEngine/TextObject.h"
#include "fbEngine/Movie.h"
#include "fbEngine/SoundSource.h"
#include "fbEngine/Sprite.h"
void TitleScene::Start()
{
	//GameObjectManager::AddNew<Movie>("movie", 1);
	//ReadCSV("");

	//背景追加
	ImageObject* titleback = GameObjectManager::AddNew<ImageObject>("TitleBack", 0);
	titleback->SetTexture(LOADTEXTURE("TitleBack.png"));
	titleback->SetPivot(0.0f, 0.0f);
	//プリーズエンター
	TextObject* text = GameObjectManager::AddNew<TextObject>("PleaseEnter", 1);
	text->Initialize(L"<PleaseEnter>", 80.0f, Color::white, SpriteEffectE::SHADOW);
	text->transform->localPosition = Vector3(WindowW/2, WindowH/2, 0);

	SoundSource *source = GameObjectManager::AddNew<SoundSource>("test", 1);
	source->Init("Asset/Sound/bgm_Title.wav");
	source->Play(true);

	enter = GameObjectManager::AddNew<SoundSource>("EnterSE", 1);
	enter->Init("Asset/Sound/TitleEnter.wav");

	PushEnter = false;
}

void TitleScene::Update()
{
	bool flag = false;
	//スタートボタンの押下確認
	FOR(PLAYER_NUM)
	{
		if (XboxInput(i)->isPushButton(XINPUT_GAMEPAD_START))
		{
			flag = true;
			break;
		}
	}
	//エンターキー
	if((flag || KeyBoardInput->isPush(DIK_RETURN)) && !PushEnter)
	{
		PushEnter = true;
		enter->Play(false);
	}

	if(PushEnter && !enter->IsPlaying())
	{
		INSTANCE(SceneManager)->ChangeScene("CharaSelectScene");
	}
}
