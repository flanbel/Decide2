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
	_Render = new RenderTarget;
	_Render->Create(Vector2(300, 300));
	_Back = GameObjectManager::AddNew<ImageObject>("BackImage",0);
	_Show = GameObjectManager::AddNew<ImageObject>("Show", 0);
	_Name = GameObjectManager::AddNew<TextObject>("Name", 0);
	_Cursor = GameObjectManager::AddNew<ImageObject>("Cursor", 3);
	_OK = GameObjectManager::AddNew<ImageObject>("OK", 3);
	_ShowModel = GameObjectManager::AddNew<ShowModel>("ShowModel", 0);
	_Sound = GameObjectManager::AddNew<SoundSource>("test", 1);

	_ShowModel->transform->localAngle = Vector3(0, 180, 0);
	_Sound->Init("Asset/Sound/Select.wav");
	
	_Name->transform->SetParent(transform);
	_Name->transform->localPosition = Vector3(50, 350, 0);
	_Name->SetBlendColor(Color::white);
	_Name->SetFontSize(40);
	_Name->SetFontStyle("HGS明朝E");
	_Name->SetString(L"notSelect");

	TEXTURE* b = LOADTEXTURE("Back.png");
	_Back->SetTexture(b);
	_Back->SetPivot(0.0f, 0.0f);
	_Back->SetClipColor(Color::red);
	_Back->transform->SetParent(transform);

	_Show->SetTexture(_Render->texture);
	_Show->SetPivot(0.0f, 0.0f);
	_Show->transform->SetParent(transform);
	_Show->transform->localPosition = Vector3(-20, 40, 0);
	_Show->SetActive(true);

	_Cursor->SetTexture(LOADTEXTURE("Cursor.png"));
	_Cursor->transform->SetParent(transform);
	_Cursor->transform->localPosition = Vector3(b->size.x / 2, b->size.y / 2, 0);
	
	_OK->SetTexture(LOADTEXTURE("OK.png"));
	_OK->transform->SetParent(transform);
	_OK->transform->localPosition = Vector3(b->size.x / 2, b->size.y / 2, 0);
	_OK->SetActive(false);

	_PlayerColor = Color::white;
	_Old = nullptr;
}

void CharaSelect::Start()
{
	_Back->SetBlendColor(_PlayerColor);
	_Cursor->SetBlendColor(_PlayerColor);
	_ShowModel->GetModel()->SetTextureBlend(_PlayerColor);
}

void CharaSelect::Update()
{
	//コントローラーが接続されているか
	bool connect = XboxInput(_PlayerIdx)->IsConnected();

	//カーソル移動
	//デバッグ
	if (true/*connect*/)
	{
		Vector3 dir = Vector3::zero;
		float speed = 4.0f;
		//コントローラー移動
		dir.x = (XboxInput(_PlayerIdx)->GetAnalog(AnalogInput::L_STICK).x / 32767.0f)*speed;
		dir.y = -(XboxInput(_PlayerIdx)->GetAnalog(AnalogInput::L_STICK).y / 32767.0f)*speed;

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
		Vector3 pos = _Cursor->transform->position;
		if (0 < pos.x + dir.x && pos.x + dir.x < WindowW)
		{
			_Cursor->transform->localPosition.x += dir.x;
		}
		if (0 < pos.y + dir.y && pos.y + dir.y < WindowW)
		{
			_Cursor->transform->localPosition.y += dir.y;
		}


		//カーソルがあっている
		if (_Info)
		{
			//新しく設定された
			if (_Info != _Old)
			{
				//モデル表示
				_Show->SetActive(true);
				_ShowModel->GetModel()->SetModelData(_Info->data);
				_ShowModel->SetAnim(_Info->anim);
				_ShowModel->GetAnim()->PlayAnimation(0,0.1f);
				_Name->SetString(_Info->name);
			}
			_ShowModel->GetAnim()->Update();
			_ShowModel->GetModel()->LateUpdate();
			//オフスクリーンレンダリング
			{
				RenderTargetManager::Instance()->ReSetRenderTarget(0, _Render, Color(0, 0, 0, 0));
				_ShowModel->GetModel()->Render();
				RenderTargetManager::Instance()->RemoveRenderTarget(0);
			}
			//決定
			if (KeyBoardInput->isPush(DIK_Z)|| XboxInput(_PlayerIdx)->isPushButton(XINPUT_GAMEPAD_B))
			{
				_Decision = !_Decision;
				//決定の時
				if (_Decision)
				{
					//ポーズアニメーション再生
					_ShowModel->GetAnim()->PlayAnimation(0, 0.1f);
					//音再生
					_Sound->Play(false);
				}
				else
				{
					//待機アニメーション再生
					_ShowModel->GetAnim()->PlayAnimation(0, 0.1f);
				}
			}
		}
		else
		{
			//カーソルが外れた
			_Name->SetString(L"notSelect");
			_Show->SetActive(false);
			_Decision = false;
			_Old = false;
		}
	}
	else
	{
		_Name->SetString(L"notSelect");
		_Show->SetActive(false);
		_Decision = false;
		_Old = false;
	}

	_Sound->Update();

	//決定しているなら
	if(_Decision)
	{
		_Back->SetBlendColor(_PlayerColor);
		_Show->SetBlendColor(Color::white);
		_OK->SetActive(true);
	}else
	{
		_Back->SetBlendColor(Color(_PlayerColor.r, _PlayerColor.g, _PlayerColor.b, 0.7f));
		_Show->SetBlendColor(Color(1, 1, 1, 0.7f));
		_OK->SetActive(false);
	}
}

Vector2 CharaSelect::GetCursorPos()
{
	Vector2 pos = Vector2(_Cursor->transform->position.x, _Cursor->transform->position.y);
	return pos;
}

void CharaSelect::SetInfo(CharacterInfo * i)
{
	_Old = _Info;
	_Info = i;
}

CharacterInfo * CharaSelect::GetInfo()
{
	return _Info;
}

void CharaSelect::Release()
{
	_ShowModel->GetModel()->SetModelData(nullptr);
	_ShowModel->SetAnim(nullptr);
}