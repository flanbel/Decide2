#include "BattleScene.h"


#include "fbEngine\_Object\_GameObject\ImageObject.h"
#include "fbEngine\_Object\_GameObject\TextObject.h"
#include "fbEngine\_Object\_GameObject\SoundSource.h"
#include "fbEngine\_Object\_Component\_3D\Camera.h"

#include "GameLight.h"
#include "GameCamera.h"
#include "GameShadowCamera.h"
#include "GameRule.h"
#include "Item.h"
#include "Player.h"
#include "Stage.h"
#include "Sky.h"

void BattleScene::Start()
{
	GameCamera* gamecamera = INSTANCE(GameObjectManager)->AddNew<GameCamera>("GameCamera", 0);

	FOR(idx,4)
	{
		char pname[20] = "Player";
		char num[2] = { (48 + idx + 1) + '\0' };
		strcat(pname, num);
		Player* p = (Player*)INSTANCE(GameObjectManager)->FindObject(pname);
		if(p)
		{
			gamecamera->AddPlayer(p);
		}
	}

	INSTANCE(GameObjectManager)->AddNew<GameLight>("GameLight", 0);
	INSTANCE(GameObjectManager)->AddNew<GameShadowCamera>("GameShadowCamera", 0);

	INSTANCE(GameObjectManager)->AddNew<Sky>("Sky", 0);
	INSTANCE(GameObjectManager)->AddNew<Stage>("Stage", 1);
	_Gong = INSTANCE(GameObjectManager)->AddNew<SoundSource>("Gong", 0);
	_BGM = INSTANCE(GameObjectManager)->AddNew<SoundSource>("BGM", 0);

	_Gong->Init("Asset/Sound/Gong.wav");
	_BGM->Init("Asset/Sound/bgm_Battle.wav");
	
	_BGM->Play(true);
	
	_GameRule = (GameRule*)INSTANCE(GameObjectManager)->FindObject("GameRule");
	_GameRule->SetDiscard(false);
	
	//タイマー作成
	if(_GameRule->GetGameRule() == GameRule::GameRuleE::TIMELIMIT)
	{
		//制限時間表示用のアレ
		_GameTimerT = INSTANCE(GameObjectManager)->AddNew<TextObject>("GameTimer", 1);
		_GameTimerT->Initialize(L"", 80.0f, Color::white, fbSprite::SpriteEffectE::SHADOW, "HGS明朝E");
		_GameTimerT->transform->SetLocalPosition(Vector3(WindowW/2, 80, 0));
		wchar_t time[10];
		int now = _GameRule->GetRemainingTime();
		Support::ToString(now, time);
		_GameTimerT->SetString(time);
		_LastTime = -1;
	}
	else
	{
		_GameTimerT = nullptr;
	}

	//カウントダウン
	_CountDownT = INSTANCE(GameObjectManager)->AddNew<TextObject>("CountDown", 1);
	_CountDownT->Initialize(L"", 160.0f, Color::red, fbSprite::SpriteEffectE::SHADOW, "HGS明朝E");
	_CountDownT->transform->SetLocalPosition(Vector3(WindowW / 2, WindowH / 2, 0.0f));

	//シーン切り替えフラグ
	_ChangeScene = false;
	//タイマー初期化
	_CountDownTimer = 0.0f;

#ifdef _DEBUG
	//影(深度)の描画
	_ShowDepth = INSTANCE(GameObjectManager)->AddNew<ImageObject>("ShowDepth", 4);
	_ShowDepth->SetTexture(INSTANCE(RenderTargetManager)->GetRTTextureFromList(RTIdxE::SHADOWDEPTH));
	_ShowDepth->SetPivot(Vector2(0, 0));
	_ShowDepth->SetActive(false);
#endif //_DEBUG
}

#include "ResultScene.h"
void BattleScene::Update()
{
#ifdef _DEBUG
	if (KeyBoardInput->isPush(DIK_C))
	{
		//深度画像表示
		_ShowDepth->SetActive(!_ShowDepth->GetActive());
	}

	//左クリック
	if (MouseInput->GetValue(MouseInE::L_CLICK))
	{
		//マウスカーソルのポジションをワールド座標に変換
		Vector3 start = INSTANCE(GameObjectManager)->mainCamera->ScreenToWorld(MouseInput->GetCursorPosOnWindow(g_MainWindow));
		//ベクトル計算
		Vector3 dir = start - INSTANCE(GameObjectManager)->mainCamera->transform->GetPosition();
		dir.Normalize();
		//終点を設定。
		Vector3 end = start + (dir * 2000.0f);
		//レイを飛ばす
		Vector3 pos = INSTANCE(PhysicsWorld)->ClosestRayTest(start, end,(const int)fbCollisionAttributeE::GROUND);
		//どこかにヒットした。
		if (pos.Length() > 0.0f)
		{
			//アイテム生成
			Item* item = INSTANCE(GameObjectManager)->AddNew<Item>("Item", 1);
			//少し上に出す
			pos.y += 10;
			item->transform->SetLocalPosition(pos);
		}
	}
#endif //_DEBUG
	//この書き方はいつかバグを生みそう
	//フェードが終わったなら
	if (!_IsFade)
	{
		//カウントダウンが終了したか？
		if (_CountDown() == true)
		{
			//時間の描画更新
			_UpdateTimer();

			//時間を進める
			//試合が終了したなら。
			if (_GameRule->IsGameSet() == GameRule::GameStateE::AFTER_GAME && !_ChangeScene)
			{
				_ChangeScene = true;
				_Gong->Play(false);
			}

			//切り替え
			if (_ChangeScene && !_Gong->IsPlaying())
			{
				//全コントローラーのバイブレーションを止める
				FOR(idx,PLAYER_NUM)
				{
					XboxInput(idx)->Vibration(0, 0);
				}
				INSTANCE(SceneManager)->ChangeScene("ResultScene");
				return;
			}
		}
	}
}

void BattleScene::_UpdateTimer()
{
	//現在の時間取得
	int nowtime = _GameRule->GetRemainingTime();
	//時間が変わった
	if (_GameTimerT != nullptr &&
		_LastTime != nowtime)
	{
		//時間の更新
		wchar_t time[10];
		Support::ToString(nowtime, time);
		_GameTimerT->SetString(time);
		//更新
		_LastTime = nowtime;
	}
}

bool BattleScene::_CountDown()
{
	float timer = 4.0f - _CountDownTimer;
	if (timer > 0.0f)
	{
		wchar_t time[10];
		if (timer > 1.0f)
		{
			//数字そのまま
			Support::ToString((int)timer, time);
		}
		else
		{
			//0ではなくGO!
			wcscpy(time, L"GO!");
		}
		
		_CountDownT->SetString(time);
		_CountDownTimer += Time::DeltaTime();
	}
	else
	{
		//文字消す
		_CountDownT->SetString(L"");
		//カウントダウン終了
		return true;
	}
	return false;
}