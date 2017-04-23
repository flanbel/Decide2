#include "CharaSelect.h"

#include "fbEngine\_Object\_GameObject\ImageObject.h"
#include "fbEngine\_Object\_GameObject\TextObject.h"
#include "fbEngine\_Object\_GameObject\SoundSource.h"

#include "ShowModel.h"
void CharaSelect::Awake()
{
	_Render = new RenderTarget;
	_Render->Create(Vector2(300, 300));
	_Back = INSTANCE(GameObjectManager)->AddNew<ImageObject>("BackImage",0);
	_Show = INSTANCE(GameObjectManager)->AddNew<ImageObject>("Show", 0);
	_Name = INSTANCE(GameObjectManager)->AddNew<TextObject>("Name", 0);
	_Cursor = INSTANCE(GameObjectManager)->AddNew<ImageObject>("Cursor", 2);
	_OK = INSTANCE(GameObjectManager)->AddNew<ImageObject>("OK", 2);
	_ShowModel = INSTANCE(GameObjectManager)->AddNew<ShowModel>("ShowModel", 0);

	_ShowModel->transform->SetLocalAngle(Vector3(0, 180, 0));
	
	_Name->transform->SetParent(transform);
	_Name->transform->SetLocalPosition(Vector3(0, 190, 0));
	_Name->Initialize(L"notSelect", 40.0f, Color::white, fbSprite::SpriteEffectE::SHADOW, "HGS明朝E");

	_Back->SetTexture(LOADTEXTURE("Back.png"));
	_Back->SetClipColor(Color::red);
	_Back->transform->SetParent(transform);

	_Cursor->SetTexture(LOADTEXTURE("Cursor.png"));
	_Cursor->transform->SetParent(transform);

	_Show->SetTexture(_Render->texture);
	_Show->transform->SetParent(transform);
	_Show->SetActive(false);
	
	_OK->SetTexture(LOADTEXTURE("OK.png"));
	_OK->transform->SetParent(transform);
	_OK->SetActive(false);

	_PlayerColor = Color::white;
	_Old = nullptr;
}

void CharaSelect::Start()
{
	_Back->SetBlendColor(_PlayerColor);
	_Cursor->SetBlendColor(_PlayerColor);
	_ShowAnim = _ShowModel->GetAnim();
	_ShowSkinModel = _ShowModel->GetModel();
	_ShowSkinModel->SetTextureBlend(_PlayerColor);
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
		float speed = 7.0f;
		//コントローラー移動
		dir.x = (XboxInput(_PlayerIdx)->GetAnalog(AnalogInputE::L_STICK).x / 32767.0f)*speed;
		dir.y = -(XboxInput(_PlayerIdx)->GetAnalog(AnalogInputE::L_STICK).y / 32767.0f)*speed;

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
		Vector3 pos = _Cursor->transform->GetPosition();
		//画面外に出ないようにチェック
		if (0 < pos.x + dir.x && pos.x + dir.x < WindowW)
		{
			Vector3 lpos = _Cursor->transform->GetLocalPosition();
			lpos.x += dir.x;
			_Cursor->transform->SetLocalPosition(lpos);
		}
		if (0 < pos.y + dir.y && pos.y + dir.y < WindowW)
		{
			Vector3 lpos = _Cursor->transform->GetLocalPosition();
			lpos.y += dir.y;
			_Cursor->transform->SetLocalPosition(lpos);
		}


		//カーソルがあっている
		if (_Info)
		{
			//新しく設定された
			if (_Info != _Old)
			{
				//モデル表示
				_Show->SetActive(true);
				_ShowSkinModel->SetModelData(_Info->Data);
				_ShowModel->SetAnim(_Info->Anim);
				_ShowAnim = _Info->Anim;
				_ShowAnim->PlayAnimation(0, 0.1f, -1);
				_Name->SetString(_Info->Name);
			}
			_ShowAnim->Update();
			_ShowSkinModel->LateUpdate();
			//オフスクリーンレンダリング
			{
				RenderTargetManager::Instance()->ReSetRT(0, _Render, Color(0, 0, 0, 0));
				_ShowSkinModel->Render();
				RenderTargetManager::Instance()->RemoveRT(0);
			}
			//決定
			if (KeyBoardInput->isPush(DIK_Z)|| XboxInput(_PlayerIdx)->IsPushButton(XINPUT_GAMEPAD_B))
			{
				_Decision = !_Decision;
				//決定の時
				if (_Decision)
				{
					//ポーズアニメーション再生
					_ShowAnim->PlayAnimation(0, 0.1f);
					//音再生
					SoundSource* selectSE = INSTANCE(GameObjectManager)->AddNew<SoundSource>("SelectSE", 1);
					selectSE->Init("Asset/Sound/Select.wav");
					selectSE->SetDelete(true);
					selectSE->Play(false);
				}
				else
				{
					//待機アニメーション再生
					_ShowAnim->PlayAnimation(0, 0.1f);
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

	//決定しているなら
	if(_Decision)
	{
		_Back->SetBlendColor(_PlayerColor);
		_Show->SetBlendColor(Color::white);
		_OK->SetActive(true);
	}else
	{
		_Back->SetBlendColor(Color(_PlayerColor.r, _PlayerColor.g, _PlayerColor.b, 0.7f));
		_Show->SetBlendColor(Color(1.0f, 1.0f, 1.0f, 0.7f));
		_OK->SetActive(false);
	}
}

Vector2 CharaSelect::GetCursorPos()
{
	Vector2 pos = Vector2(_Cursor->transform->GetPosition().x, _Cursor->transform->GetPosition().y);
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
	_ShowSkinModel->SetModelData(nullptr);
	_ShowModel->SetAnim(nullptr);
}