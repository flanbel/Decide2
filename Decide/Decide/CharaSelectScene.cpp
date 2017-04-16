#include "CharaSelectScene.h"
#include "fbEngine/ImageObject.h"
#include "fbEngine/TextObject.h"
#include "CharaSelect.h"
#include "CharaRingFrame.h"
#include "Character.h"
#include "fbEngine/SkinModelData.h"
#include "fbEngine/Animation.h"
#include "SelectCamera.h"
#include "GameLight.h"
#include "GameShadowCamera.h"
#include "Player.h"
#include "BattleScene.h"
#include "GameRule.h"

#include "fbEngine/CircleCollision.h"
#include "fbEngine/Sprite.h"

namespace
{
	//各キャラクターの名前
	const wchar_t* name[] = {
		L"TVman",
		L""
	};
	//各キャラクターのモデルファイル名	 \Asset/Xfile から先のパス
	const char* path[] = {
		"TV/TVman1.X",
		""
	};
	//キャラクターのパラメーター
	const CharacterParam param[] =
	{
		{ 130.0f,2.0f,1.2f,1.0f },
		{ 0.0f,0.0f,0.0f,0.0f },
	};
}

void CharaSelectScene::Start()
{
	INSTANCE(GameObjectManager)->AddNew<SelectCamera>("camera", 0);
	INSTANCE(GameObjectManager)->AddNew<GameLight>("GameLight", 0);
	INSTANCE(GameObjectManager)->AddNew<GameShadowCamera>("GameShadowCamera", 0);
	//ゲームルール
	_GameRule = INSTANCE(GameObjectManager)->AddNew<GameRule>("GameRule", 0);
	_GameRule->SetDiscard(false);
	//ゲームルール表示用テキスト
	_DisplayGameRule = INSTANCE(GameObjectManager)->AddNew<TextObject>("DisplayGameRule", 1);
	_DisplayGameRule->Initialize(L"GameRule  TimeLimit 2", 40.0f, Color::white,
		fbSprite::SpriteEffectE::SHADOW, "HGS明朝E",fbText::TextFormatE::LEFT);
	_DisplayGameRule->transform->SetLocalPosition(Vector3(50, 100, 0));
	//背景画像
	ImageObject* backGround = INSTANCE(GameObjectManager)->AddNew<ImageObject>("Back",0);
	backGround->SetTexture(LOADTEXTURE("SelectBack.png"));
	backGround->SetPivot(0.0f, 0.0f);
	//レディファイト
	_ReadyFight = INSTANCE(GameObjectManager)->AddNew<ImageObject>("ReadyToFight", 4);
	_ReadyFight->SetTexture(LOADTEXTURE("ReadyToStart.png"));
	_ReadyFight->transform->SetLocalPosition(Vector3(WindowW/2, WindowH/2, 0));
	_ReadyFight->SetActive(false);

	//キャラクターフレーム作成
	_CreateCharaFrame(name, path, param);
	
	//各プレイヤーの色
	const Color playercolor[PLAYER_NUM] =
	{
		Color::red,		//1P
		Color::blue,	//2P
		Color::yellow,	//3P
		Color::green	//4P
	};

	//選択のやつを生成
	FOR(idx,PLAYER_NUM)
	{
		_SelectArray[idx] = INSTANCE(GameObjectManager)->AddNew<CharaSelect>("Select", 0);
		_SelectArray[idx]->SetIdx(idx);
		_SelectArray[idx]->SetColor(playercolor[idx]);
		float Space = 300.0f;
		_SelectArray[idx]->transform->SetLocalPosition(Vector3(190.0f+(idx*Space), 450.0f, 0));
	}

	//シーン切り替えフラグ初期化
	_ChangeScene = false;
}

void CharaSelectScene::Update()
{
	//キャラクター選択(こいつがするの？？？？？？)
	FOR(idx,PLAYER_NUM)
	{
		//jはキャラクター数
		short chara = 0;
		FOR(nul,CHARACTER_NUM)
		{
			//各カーソルと重なっているか判定
			if(_RingArray[chara]->Judgment(_SelectArray[idx]->GetCursorPos()))
			{
				//リングの情報セット
				_SelectArray[idx]->SetInfo(_RingArray[chara]->GetInfo(idx));
				
				break;
			}
			chara++;
		}
		//何もセットしない
		if(chara == CHARACTER_NUM && _SelectArray[idx]->GetDecision() == false)
		{
			_SelectArray[idx]->SetInfo(nullptr);
		}
	}
	//ゲームルール更新
	_UpdateGameRule();
	

	//決定しているプレイヤー数カウント
	int count = 0;
	FOR(idx, PLAYER_NUM)
	{
		if (_SelectArray[idx]->GetDecision())
			count++;
	}

	//二人以上選択されている
	if (count >= 2)
	{
		//スタートボタンの押下確認
		bool flag = INSTANCE(InputManager)->IsPushButtonAll(XINPUT_GAMEPAD_START);

		//ReadyToFight表示
		_ReadyFight->SetActive(true);
		//戦闘シーンへ移行
		if (KeyBoardInput->isPush(DIK_RETURN) || flag && !_ChangeScene)
		{
			_ChangeScene = true;
			SetFade(true);
		}
	}else
	{
		_ReadyFight->SetActive(false);
	}

	//シーン切り替え
	if(_ChangeScene && !_IsFade)
	{
		vector<Player*> Array;
		FOR(idx,PLAYER_NUM)
		{
			//選択されているならプレイヤーオブジェクト生成
			if (_SelectArray[idx]->GetDecision())
			{
				char pname[20] = "Player";
				char num[2] = { (48 + idx + 1) + '\0' };
				strcat(pname, num);
				//プレイヤー生成
				Player* p = INSTANCE(GameObjectManager)->AddNew<Player>(pname, 1);
				p->SetIdx(idx);
				p->SetInfo(_SelectArray[idx]->GetInfo());
				p->SetColor(_SelectArray[idx]->GetColor());
				//シーン切り替えしても破棄しないように設定
				p->SetDiscard(false,true);
				Array.push_back(p);
				//ゲームルールの方にプレイヤーの情報をセット
				_GameRule->SetPlayer(p, idx);
			}
		}
		//選択のやつ解放
		FOR(idx,PLAYER_NUM)
		{
			_SelectArray[idx]->Release();
		}
		//切り替え
		INSTANCE(SceneManager)->ChangeScene("BattleScene");
		return;
	}
}

void CharaSelectScene::_CreateCharaFrame(const wchar_t ** nameArray,const char ** pathArray,const CharacterParam* paramArray)
{
	//キャラクタ数分フレーム作成
	FOR (chara,CHARACTER_NUM)
	{
		_RingArray[chara] = INSTANCE(GameObjectManager)->AddNew<CharaRingFrame>("Frame", 0);
		//今は一体しかいないので場所は適当
		_RingArray[chara]->transform->SetLocalPosition(Vector3(WindowW / 2, 100, 0));
		//プレイヤ数分情報作成
		FOR(idx,PLAYER_NUM)
		{
			//このへんメモリリーク注意
			Animation* anim = new Animation(nullptr, nullptr);
			SkinModelData* data = new SkinModelData();
			data->CloneModelData(SkinModelManager::LoadModel(pathArray[chara]), anim);
			CharacterInfo* info = new CharacterInfo(nameArray[chara], pathArray[chara], &paramArray[chara], data, anim);
			_RingArray[chara]->SetInfo(info, idx);
		}
	}
}

void CharaSelectScene::_UpdateGameRule()
{
	//現在のルール取得
	GameRule::GameRuleE rule = _GameRule->GetGameRule();
	//設定されている値取得
	int value = _GameRule->GetValue();
	//変更されたか？
	bool change = false;
	//ゲームルール変更
	if (KeyBoardInput->isPush(DIK_UP))
	{
		rule = (GameRule::GameRuleE)((rule + 1) % GameRule::GameRuleE::NUM);
		value = 3;
		change = true;
	}
	if (KeyBoardInput->isPush(DIK_DOWN))
	{
		rule = (GameRule::GameRuleE)((rule - 1) < 0 ? (GameRule::GameRuleE::NUM - 1) : (rule - 1));
		value = 3;
		change = true;
	}
	//値変更
	if (KeyBoardInput->isPush(DIK_RIGHT))
	{
		value = min(value + 1, 99);
		change = true;
	}
	if (KeyBoardInput->isPush(DIK_LEFT))
	{
		value = max(value - 1, 1);
		change = true;
	}
	//変更されているなら
	if (change)
	{
		//ゲームルール更新
		_GameRule->SetGameRule(rule, value);
		//表示用テキスト更新
		wchar_t display[128];			//最終的な
		wchar_t *ruleW = NULL;			//ゲームルール
		wchar_t valueW[5] = { L'0' };	//値
		switch (rule)
		{
		case GameRule::STOCK:
			ruleW = L"Stock ";
			break;
		case GameRule::TIMELIMIT:
			ruleW = L"TimeLimit ";
			break;
		case GameRule::KNOCKOUT:
			ruleW = L"KnockOut ";
			break;
		default:
			ruleW = L"Unknown ";
			break;
		}
		//文字列に変換
		Support::ToString(value, valueW);
		//文字列コピー
		wcscpy_s(display, wcslen(L"GameRule  ") + 1, L"GameRule  ");
		//文字列追加
		wcscat_s(display, wcslen(display) + wcslen(ruleW) + 1, ruleW);
		wcscat_s(display, wcslen(display) + wcslen(valueW) + 1, valueW);
		//表示更新
		_DisplayGameRule->SetString(display);
	}
}