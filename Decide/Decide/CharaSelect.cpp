#include "CharaSelect.h"
#include "fbEngine/ImageObject.h"
#include "fbEngine/TextObject.h"
#include "ShowModel.h"
#include "fbEngine/SkinModel.h"
#include "fbEngine/Animation.h"
#include "fbEngine/RenderTargetManager.h"
#include "fbEngine/SoundSource.h"
void CharaSelect::Awake()
{
	render = new RenderTarget;
	render->Create(Vector2(300, 300));
	back = GameObjectManager::AddNew<ImageObject>("back",0);
	show = GameObjectManager::AddNew<ImageObject>("show", 0);
	name = GameObjectManager::AddNew<TextObject>("name", 0);
	cursor = GameObjectManager::AddNew<ImageObject>("cursor", 3);
	showmodel = GameObjectManager::AddNew<ShowModel>("show", 0);
	sound = GameObjectManager::AddNew<SoundSource>("test", 1);

	showmodel->transform->localAngle = Vector3(0, 180, 0);
	sound->Init("Asset/Sound/Select.wav");
	
	name->transform->SetParent(transform);
	name->transform->localPosition = Vector3(50, 350, 0);
	name->SetBlendColor(Color::white);
	name->SetFontSize(40);
	name->SetFontStyle("HGS明朝E");
	name->SetString(L"notSelect");

	TEXTURE* b = TextureManager::LoadTexture("back.png");
	back->SetTexture(b);
	back->transform->SetParent(transform);

	show->SetTexture(render->texture);
	show->transform->SetParent(transform);
	show->transform->localPosition = Vector3(-20, 40, 0);
	show->SetActive(true);

	TEXTURE* c = TextureManager::LoadTexture("cursor.png");
	c->pivot = Vector2(0.5, 0.5);
	cursor->SetTexture(c);

	cursor->transform->SetParent(transform);
	cursor->transform->localPosition = Vector3(b->size.x / 2, b->size.y / 2, 0);

	playerColor = Color::white;
	old = nullptr;
}

void CharaSelect::Start()
{
	back->SetBlendColor(playerColor);
	cursor->SetBlendColor(playerColor);
	showmodel->GetModel()->SetTextureBlend(playerColor);
}

void CharaSelect::Update()
{
	//コントローラーが接続されているか
	bool connect = XboxInput(playerIdx)->IsConnected();

	//カーソル移動
	//デバッグ
	if (true/*connect*/)
	{
		Vector3 dir = Vector3::zero;
		float speed = 4.0f;
		//コントローラー移動
		dir.x = (XboxInput(playerIdx)->GetAnalog(AnalogInput::L_STICK).x / 32767.0f)*speed;
		dir.y = -(XboxInput(playerIdx)->GetAnalog(AnalogInput::L_STICK).y / 32767.0f)*speed;

		if (KeyBoardInput->isPressed(DIK_W))
		{
			dir.y -= speed;
		}
		if (KeyBoardInput->isPressed(DIK_S))
		{
			dir.y += speed;
		}
		if (KeyBoardInput->isPressed(DIK_D))
		{
			dir.x += speed;
		}
		if (KeyBoardInput->isPressed(DIK_A))
		{
			dir.x -= speed;
		}
		Vector3 pos = cursor->transform->position;
		if (0 < pos.x + dir.x && pos.x + dir.x < WindowW)
		{
			cursor->transform->localPosition.x += dir.x;
		}
		if (0 < pos.y + dir.y && pos.y + dir.y < WindowW)
		{
			cursor->transform->localPosition.y += dir.y;
		}


		//カーソルがあっている
		if (info)
		{
			//新しく設定された
			if (info != old)
			{
				//モデル表示
				show->SetActive(true);
				showmodel->GetModel()->SetModelData(info->data);
				showmodel->SetAnim(info->anim);
				showmodel->GetAnim()->PlayAnimation(0,0.1f);
				name->SetString(info->name);
			}
			showmodel->GetAnim()->Update();
			showmodel->GetModel()->LateUpdate();
			//オフスクリーンレンダリング
			{
				RenderTargetManager::Instance()->ReSetRenderTarget(0, render, Color(0, 0, 0, 0));
				showmodel->GetModel()->Render();
				RenderTargetManager::Instance()->RemoveRenderTarget(0);
			}
			//決定
			if (KeyBoardInput->isPush(DIK_Z)|| XboxInput(playerIdx)->isPushButton(XINPUT_GAMEPAD_B))
			{
				decision = !decision;
				//決定の時
				if (decision)
				{
					//ポーズアニメーション再生
					showmodel->GetAnim()->PlayAnimation(0, 0.1f);
					//音再生
					sound->Play(false);
				}
				else
				{
					//待機アニメーション再生
					showmodel->GetAnim()->PlayAnimation(0, 0.1f);
				}
			}
		}
		else
		{
			//カーソルが外れた
			name->SetString(L"notSelect");
			show->SetActive(false);
			decision = false;
			old = false;
		}
	}
	else
	{
		name->SetString(L"notSelect");
		show->SetActive(false);
		decision = false;
		old = false;
	}

	sound->Update();

	//決定しているなら
	if(decision)
	{
		back->SetBlendColor(playerColor);
		show->SetBlendColor(Color::white);
	}else
	{
		back->SetBlendColor(Color(playerColor.r, playerColor.g, playerColor.b, 0.7f));
		show->SetBlendColor(Color(1, 1, 1, 0.7f));
	}
}

Vector2 CharaSelect::GetCursorPos()
{
	Vector2 pos = Vector2(cursor->transform->position.x, cursor->transform->position.y);
	return pos;
}

void CharaSelect::SetInfo(CharacterInfo * i)
{
	old = info;
	info = i;
}

CharacterInfo * CharaSelect::GetInfo()
{
	return info;
}

void CharaSelect::Release()
{
	showmodel->GetModel()->SetModelData(nullptr);
	showmodel->SetAnim(nullptr);
}