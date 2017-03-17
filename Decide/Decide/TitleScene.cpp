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
	//プリーズエンターテキスト
	TextObject* text = GameObjectManager::AddNew<TextObject>("PleaseEnter", 1);
	text->Initialize(L"<PleaseEnter>", 80.0f, Color::white, sprite::SpriteEffectE::SHADOW);
	text->transform->localPosition = Vector3(WindowW/2, WindowH/2, 0);
	//BGM
	SoundSource *bgm = GameObjectManager::AddNew<SoundSource>("BGM", 1);
	bgm->Init("Asset/Sound/bgm_Title.wav");
	bgm->Play(true);
	//決定時の効果音
	_EnterSE = GameObjectManager::AddNew<SoundSource>("EnterSE", 1);
	_EnterSE->Init("Asset/Sound/TitleEnter.wav");
	//シーン切り替えフラグ初期化
	_ChangeScene = false;
}

void TitleScene::Update()
{
	//スタートボタンの押下確認
	bool flag = INSTANCE(InputManager)->IsPushButtonAll(XINPUT_GAMEPAD_START);

	//エンターキー
	if((flag || KeyBoardInput->isPush(DIK_RETURN)) && !_ChangeScene)
	{
		_ChangeScene = true;
		_EnterSE->Play(false);
		SetFade(true);
	}

	if(_ChangeScene &&	//エンターキーが押された
		!_IsFade)		//フェード終了
	{
		INSTANCE(SceneManager)->ChangeScene("CharaSelectScene");
		return;
	}
}
