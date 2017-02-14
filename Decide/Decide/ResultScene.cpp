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

	SoundSource *sound = GameObjectManager::AddNew<SoundSource>("test", 1);
	sound->Init("Asset/Sound/Result.wav");
	sound->Play(false);
}

void ResultScene::Update()
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
	if(KeyBoardInput->isPush(DIK_RETURN) || flag)
	{
		INSTANCE(SceneManager)->ChangeScene("CharaSelectScene");
		return;
	}
}

void ResultScene::SetList()
{
}

void ResultScene::Setrank(vector<int> r)
{
	{
		rank = r;
		FOR(rank.size())
		{
			TextObject* rt = GameObjectManager::AddNew<TextObject>("rt", 0);
			rt->transform->localPosition = Vector3(300, 150 * i + 150, 0);
			rt->SetFontStyle("HGS–¾’©E");
			wchar_t s[128];
			wchar_t junni[2] = {48+(i+1),0};
			wcscpy_s(s, 2,junni);
			wcscat_s(s, 128, L"ˆÊPlayer");
			wchar_t idx[2] = { 48 + rank[i]+1,0 };
			wcscat_s(s, 128, idx);
			rt->SetString(s);
			rt->SetBlendColor(Color::white);
			rt->SetFontSize(80);
		}
	}
}
