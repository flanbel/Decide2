#include "Player.h"
#include "fbEngine/Skinmodel.h"
#include "fbEngine/SkinmodelData.h"
#include "fbEngine/Animation.h"
#include "fbEngine/RigidBody.h"
#include "fbEngine/BoxCollider.h"

#include "PlayerParameter.h"
#include "fbEngine/ParticleEmitter.h"
#include "fbEngine/PlatePrimitive.h"
#include "fbEngine/FontManager.h"
#include "fbEngine/SoundSource.h"
#include "Item.h"
#include "GameRule.h"

namespace
{
	const ParicleParameter parm[2] =
	{
		{
			"SmokeBall.png",	//パス
			Vector3(0,00,0),		//初速
			Vector2(70,70),		//サイズ
			0.5f,				//寿命
			0.08f,				//発生時間
			Vector3(30,0,30),	//ポジションランダム
			Vector3(0,0,0),		//初速度ランダム
			Vector3(0,0,0),		//速度ランダム
			Vector3::zero,		//重力
			true,				//フェード
			0.3f,				//フェード時間
			0.5f,				//初期α
			true,				//ビルボード
			1.0f,				//ブライト
			0,					//αブレンド
			Color(1.5f,1.5f,1.5f,1.0f)	//乗算カラー
		},
		{
			"Fire1.png",	//パス
			Vector3(0,40,0),		//初速
			Vector2(30,30),		//サイズ
			1.0f,				//寿命
			0.01f,				//発生時間
			Vector3(30,0,30),	//ポジションランダム
			Vector3(5,5,5),		//初速度ランダム
			Vector3(0,0,0),		//速度ランダム
			Vector3::zero,		//重力
			true,				//フェード
			0.3f,				//フェード時間
			1.0f,				//初期α
			true,				//ビルボード
			1.0f,				//ブライト
			1,					//αブレンド
			Color(2.5f,2.5f,1.0f,1.0f)	//乗算カラー
		}
	};

}

Player::Player(const char * name) :
	GameObject(name),
	_Damage(0),
	_Stock(1),
	_LastAttack(-1),
	_Jump(false),
	_IsAction(false),
	_Gravity(0)
	/*StayState(this),
	RunState(this),
	WalkState(this),
	JumpState(this),
	FallState(this)*/
{

}

void Player::Awake()
{
	//各コンポーネント追加
	_Model = AddComponent<SkinModel>();
	_Rigid = AddComponent<RigidBody>();
	BoxCollider* coll = AddComponent<BoxCollider>();
	_Anim = AddComponent<Animation>();

	_Height = 100;
	coll->Create(Vector3(25, _Height, 25));
	_Rigid->Create(1, coll, Collision_ID::PLAYER, Vector3::zero, Vector3(0, _Height / 2, 0));
	_RB = (btRigidBody*)_Rigid->GetCollisonObj();
	//RigidBodyの上下の移動量を消す
	_RB->setGravity(btVector3(0.0f, 0.0f, 0.0f));
	//スリープさせない(必要かどうかわからない。)
	_RB->setSleepingThresholds(0, 0);
	//ゲームルール取得
	_GameRule = (GameRule*)GameObjectManager::FindObject("GameRule");
	//ストック数取得
	_Stock = _GameRule->GetStock();

	//パラメータ生成
	_Pparameter = GameObjectManager::AddNew<PlayerParameter>("_Pparameter", 0);
	_Pparameter->SetStock(_Stock);
	_Pparameter->SetDamage(_Damage);
	_Pparameter->Discard(false);
	//プレート生成
	_IdxPlate = GameObjectManager::AddNew<PlatePrimitive>("plate", 4);
	_IdxPlate->Discard(false);
	//王冠生成
	_CrownPlate = GameObjectManager::AddNew<PlatePrimitive>("Crown", 4);
	_CrownPlate->transform->SetParent(_IdxPlate->transform);
	_CrownPlate->transform->localPosition = Vector3(10, 60, 0);
	_CrownPlate->SetTexture(LOADTEXTURE("Crown.png"));
	_CrownPlate->Discard(false);

	_PlayerColor = Color::white;
	//いろいろなパラメータ初期化
	_Reset();



	_Smoke = GameObjectManager::AddNew<ParticleEmitter>("SmokeEmit", 2);
	_Smoke->Discard(false);
	_Smoke->transform->SetParent(transform);
	_Smoke->Init(parm[0]);
	_Smoke->SetEmitFlg(false);

	_Fire = GameObjectManager::AddNew<ParticleEmitter>("FireEmit", 2);
	_Fire->Discard(false);
	_Fire->transform->SetParent(transform);
	_Fire->Init(parm[1]);
	_Fire->SetEmitFlg(false);
}

void Player::Start()
{
	//パンチの効果音を鳴らすオブジェクト
	//オブジェクトっておかしくね？
	_PunchSound = GameObjectManager::AddNew<SoundSource>("PunchSound", 0);
	_PunchSound->Init("Asset/Sound/punch.wav");

	float space = 300.0f;
	_Pparameter->transform->localPosition = Vector3(180.0f + (_Playeridx * space), 600.0f, 0.0f);
	_Model->SetCamera(GameObjectManager::mainCamera);
	_Model->SetLight(GameObjectManager::mainLight);

	char path[10];
	Support::ToString(_Playeridx + 1, path);
	strcat_s(path, strlen(path) + strlen("P.png") + 1, "P.png");
	_IdxPlate->SetTexture(LOADTEXTURE(path));
	_IdxPlate->SetBlendColor(_PlayerColor);
	_CrownPlate->Start();
	_CrownPlate->SetBlendColor((Color)Color::yellow * 4.0f);
	_CrownPlate->SetActive(false);

	_Pparameter->SetColor(_PlayerColor);
	_Model->SetTextureBlend(_PlayerColor * 8.0f);
}

#include "DamageCollision.h"
#include "AttackCollision.h"
#include "fbEngine/Camera.h"
void Player::Update()
{
	//開始前じゃないなら
	if (_GameRule->GetGameState() != GameRule::GameStateE::BEFORE_GAME)
	{
		//UpdateStateMachine();

		ItemAction();
		Move();

		//n割り再生したら遷移開始
		//ステートパターン実装したい。
		if (_IsAction)
		{
			float normalTime;
			normalTime = 0.7f;
			if (_State == PStateE::KICK &&
				_Anim->GetTimeRatio() > normalTime)
			{
				ChangeState(PStateE::STAY);
			}

			if (_State == PStateE::PUNCH &&
				_Anim->GetTimeRatio() > normalTime)
			{
				ChangeState(PStateE::STAY);
			}

			normalTime = 0.8f;
			if (_State == PStateE::SLASH)
			{
				//前に移動(適当)
				_Move.Add(transform->Direction(Vector3(0, 0, 5.0f)));
				if (_Anim->GetTimeRatio() > normalTime)
				{
					ChangeState(PStateE::STAY);
				}
			}
		}

		Attack();
		Damage();

		Jump();
		Blown();
	}

	//Y軸の移動量保存
	float moveY = _Move.y;
	//Y軸移動はしない
	_Move.y = 0.0f;
	if (_Move.Length() > 0)
	{
		//XZ軸の移動
		transform->localPosition.Add(_Move);
		transform->UpdateTransform();
	}

	_GravityCheck(moveY);

	AnimationControl();

	//場外にでたときの処理。
	Death();
}

void Player::LateUpdate()
{
	//頭の上あたり
	_IdxPlate->transform->localPosition = transform->LocalPos(Vector3(-50, 150, 0));
	_IdxPlate->transform->Update();
	_IdxPlate->transform->LockAt(GameObjectManager::mainCamera->gameObject);

	_CrownPlate->transform->LockAt(GameObjectManager::mainCamera->gameObject);
}

void Player::UpdateStateMachine()
{
	////nullチェック
	//if(currentState != nullptr)
	//{
	//	currentState->Update();
	//}
}

void Player::ChangeState(PStateE next)
{
	//if(currentState != nullptr)
	//{
	//	//ステートを抜ける
	//	currentState->Leave();
	//}
	//switch (next)
	//{
	//case PStateE::STAY:
	//	currentState = &StayState;
	//	break;
	//case PStateE::WALK:
	//	currentState = &WalkState;
	//	break;
	//case PStateE::DASH:
	//	currentState = &RunState;
	//	break;
	//case PStateE::JUMP:
	//	currentState = &JumpState;
	//	break;
	//case PStateE::FALL:
	//	currentState = &FallState;
	//	break;
	//case PStateE::PUNCH:
	//	currentState = &StayState;
	//	break;
	//case PStateE::KICK_CHARGE:
	//	currentState = &StayState;
	//	break;
	//case PStateE::KICK:
	//	currentState = &StayState;
	//	break;
	//case PStateE::SLASH:
	//	currentState = &StayState;
	//	break;
	//case PStateE::DAMAGE:
	//	currentState = &StayState;
	//	break;
	//case PStateE::BLOW:
	//	currentState = &StayState;
	//	break;
	//default:
	//	//エラー
	//	break;
	//}
	//_State = next;
	//currentState->Enter();

	//今のステートを抜けるときの処理
	switch (_State)
	{
	case PStateE::STAY:
		break;
	case PStateE::WALK:
		break;
	case PStateE::DASH:
		_Smoke->SetEmitFlg(false);
		break;
	case PStateE::JUMP:
		break;
	case PStateE::FALL:
		break;
	case PStateE::PUNCH:
		_IsAction = false;
		break;
	case PStateE::KICK_CHARGE:
		_IsAction = false;
		_Fire->SetEmitFlg(false);
		break;
	case PStateE::KICK:
		_IsAction = false;
		break;
	case PStateE::SLASH:
		_IsAction = false;
		break;
	case PStateE::DAMAGE:
		break;
	case PStateE::BLOW:
		_Smoke->SetEmitFlg(false);
		break;
	default:
		break;
	}
	//次のステートに切り替わるときの処理
	switch (next)
	{
	case PStateE::STAY:
		break;
	case PStateE::WALK:
		break;
	case PStateE::DASH:
		_Smoke->SetEmitFlg(true);
		break;
	case PStateE::JUMP:
		break;
	case PStateE::FALL:
		break;
	case PStateE::PUNCH:
		_IsAction = true;
		break;
	case PStateE::KICK_CHARGE:
		_IsAction = true;
		break;
	case PStateE::KICK:
		_IsAction = true;
		break;
	case PStateE::SLASH:
		_IsAction = true;
		break;
	case PStateE::DAMAGE:
		break;
	case PStateE::BLOW:
		_Smoke->SetEmitFlg(true);
		break;
	default:
		break;
	}
	_State = next;
}

void Player::PlayAnimation(int idx, float time, int loop)
{
	//異なるアニメーションの時のみ再生
	if (_Anim->GetPlayAnimNo() != idx)
	{
		_Anim->PlayAnimation(idx, time, loop);
	}
}

void Player::AnimationControl()
{
	switch (_State)
	{
	case PStateE::STAY:
		//待機モーション
		PlayAnimation(0, 0.2f);
		break;
	case PStateE::WALK:
		PlayAnimation(1, 0.2f);
		break;
	case PStateE::DASH:
		PlayAnimation(2, 0.2f);
		break;
	case PStateE::JUMP:
		//NONE
		break;
	case PStateE::FALL:
		//降下中モーション
		PlayAnimation(9, 0.2f);
		break;
	case PStateE::PUNCH:
		//パンチ再生
		PlayAnimation(3, 0.2f, 1);
		_Anim->SetAnimeSpeed(2.0f);
		break;
	case PStateE::KICK_CHARGE:
		//蹴りため開始
		PlayAnimation(4, 0.2f, 1);
		break;
	case PStateE::KICK:
		PlayAnimation(5, 0.1f, 1);
		break;
	case PStateE::SLASH:
		//武器振り再生
		PlayAnimation(8, 0.2f, 1);
		_Anim->SetAnimeSpeed(1.5f);
		break;
	case PStateE::DAMAGE:
		PlayAnimation(6, 0.2f, 1);
		break;
	case PStateE::BLOW:
		PlayAnimation(7, 0.2f, 1);
		break;
	default:
		break;
	}
}

void Player::Attack()
{
	if (!_IsAction)
	{
		//Bボタン
		if ((KeyBoardInput->isPush(DIK_Z) || XboxInput(_Playeridx)->IsPushButton(XINPUT_GAMEPAD_B)))
		{
			//アイテムを持っている
			if (_HaveItem)
			{
				ChangeState(PStateE::SLASH);
			}
			//パンチ
			else
			{
				ChangeState(PStateE::PUNCH);
			}
		}

		//Aボタン
		if ((KeyBoardInput->isPush(DIK_SPACE) || XboxInput(_Playeridx)->IsPushButton(XINPUT_GAMEPAD_A)))
		{
			ChangeState(PStateE::KICK_CHARGE);
		}
	}

	//チャージ中
	if (_State == PStateE::KICK_CHARGE)
	{
		//フルチャージ
		if (_Anim->GetPlaying() == false)
		{
			_Fire->SetEmitFlg(true);
		}
		//ボタンが離された
		if (!KeyBoardInput->isPressed(DIK_SPACE) && !XboxInput(_Playeridx)->IsPressButton(XINPUT_GAMEPAD_A))
		{
			double rate = _Anim->GetTimeRatio();
			//フルチャージなら1.5倍
			_CharagePower = (rate >= 1.0f) ? 1.5f : rate;
			ChangeState(PStateE::KICK);
		}
	}


	if (_IsAction)
	{
		//現在のフレーム取得
		const int nowFrame = _Anim->NowFrame();

		//移動量が吹き飛び値の補正になる
		Vector3 V = _Dir;
		V.Scale(0.4f);

		//パンチの攻撃判定
		if (_State == PStateE::PUNCH &&
			nowFrame == 8)
		{
			_PunchSound->Play(false);
			//あたり判定を出す
			AttackCollision* obj = GameObjectManager::AddNew<AttackCollision>("panch", 1);
			Transform* t = obj->GetComponent<Transform>();
			//前に出す
			t->localPosition = transform->Local(Vector3(0, 60, 30));
			t->localAngle = transform->localAngle;
			t->UpdateTransform();

			DamageCollision::DamageCollisonInfo info;
			//攻撃力補正
			info.Damage = Random::Range(2, 3) * _Cparameter->Power;
			info.Blown = transform->Direction(Vector3(0.0f, 60.0f, 60.0f) + V);
			//吹き飛ばし補正
			info.Blown.Scale(_Cparameter->BlowCorrection);
			info.Rigor = 0.2f;
			info.StopTime = 5;
			float life = 0.2f;
			obj->Create(_Playeridx, life, Vector3(50, 50, 50), info);
		}

		//キックの攻撃判定
		if (_State == PStateE::KICK &&
			nowFrame == 15)
		{
			_PunchSound->Play(false);
			//あたり判定を出す
			AttackCollision* obj = GameObjectManager::AddNew<AttackCollision>("kick", 1);
			Transform* t = obj->GetComponent<Transform>();
			//あたり判定の大きさ
			Vector3 size = Vector3(60, 60, 80);

			//前に出す
			t->localPosition = transform->Local(Vector3(0, 60, (size.z / 2) + 10));
			t->localAngle = transform->localAngle;
			t->UpdateTransform();

			DamageCollision::DamageCollisonInfo info;
			//攻撃力補正
			info.Damage = (Random::Range(7, 10) * (_CharagePower + 0.2f)) * _Cparameter->Power;
			info.Blown = transform->Direction(Vector3(0.0f, 120.0f, 180.0f) + V);
			//吹き飛ばし補正
			info.Blown.Scale(_Cparameter->BlowCorrection * (_CharagePower + 0.5f));
			info.Rigor = 0.6f;
			info.StopTime = 10;
			float life = 0.2f;
			obj->Create(_Playeridx, life, size, info);
		}

		//剣の攻撃判定
		if (_State == PStateE::SLASH &&
			nowFrame <= 30 && (nowFrame % 5) == 0)
		{
			//あたり判定を出す
			AttackCollision* obj = GameObjectManager::AddNew<AttackCollision>("sword", 1);
			Transform* t = obj->GetComponent<Transform>();
			//前に出す
			/*t->SetParent(transform);
			t->localPosition = Vector3(0, 60, 30);*/
			t->localPosition = transform->Local(Vector3(0, 60, 30));
			t->localAngle = transform->localAngle;
			t->UpdateTransform();
			Vector3 size = Vector3(70, 70, 40);

			DamageCollision::DamageCollisonInfo info;
			//攻撃力補正
			info.Damage = Random::Range(1, 2) * _Cparameter->Power;
			info.Blown = transform->Direction(Vector3(60.0f, 90.0f, 0.0f) + V);
			//吹き飛ばし補正
			info.Blown.Scale(_Cparameter->BlowCorrection);
			info.Rigor = 0.1f;
			info.StopTime = 1;
			float life = 0.01f;
			obj->Create(_Playeridx, life, size, info);
		}
	}
}

void Player::Damage()
{
	//ダメージ処理
	FOR(PLAYER_NUM)
	{
		//自分のあたり判定はしない
		if (i == _Playeridx)
			continue;

		const DamageCollision *co = (const DamageCollision*)PhysicsWorld::Instance()->FindOverlappedDamageCollision(_RB, i);
		if (co)
		{
			//吹き飛ぶ方向
			Vector3 vec = co->info.Blown;
			//反対を向く
			vec.Scale(-1);
			vec.y = 0;
			//正規化
			vec.Normalize();
			//ベクトルから角度を求める
			float rot = D3DXToRadian(360) - atan2f(vec.z, vec.x);
			//回転
			transform->localAngle.y = D3DXToDegree(rot + D3DXToRadian(90));

			//最後に攻撃してきたとしてやつ保存
			//とりあえず永続
			_LastAttack = i;
			//ダメージ加算(999が最大)
			_Damage = min(999, _Damage + co->info.Damage);
			//テキストのダメージ更新
			_Pparameter->SetDamage(_Damage);
			//タイマー初期化
			_RigorTimer = 0;
			//硬直時間受け取り
			_Rigor = co->info.Rigor;
			//吹き飛び力受け取り
			_Blown = co->info.Blown;
			//吹き飛び補正
			_Blown.Scale(_Cparameter->ToBlowCorrection);
			//更にダメージによる吹き飛び補正
			_Blown.Scale(1.0f + (float)_Damage / 30.0f);


			//吹き飛ばし力があるなら
			if (_Blown.Length() > 0)
			{
				//コントローラ振動
				XboxInput(_Playeridx)->Vibration(20000, 20000);
				//吹き飛んだのでジャンプ回数を1に
				_JumpCount = 1;
			}
		}
	}

}

void Player::Blown()
{
	//吹き飛ばし力があるのなら。
	if (_Blown.Length() > 0)
	{
		//前フレームとの時間の差
		const float delta = Time::DeltaTime();
		//ぶっ飛び計算
		_RigorTimer += delta;
		//硬直時間以下
		if (_RigorTimer <= _Rigor)
		{
			//吹き飛び
			_Move.Add(_Blown * delta);
			ChangeState(PStateE::BLOW);
		}
		else
		{
			//超過した時間
			float overtime = _RigorTimer - _Rigor;
			//失われし時
			float losttime = delta - overtime;
			//吹き飛ばし
			_Blown.Scale(losttime);
			_Move.Add(_Blown);
			//吹き飛ばしを0に
			_Blown = Vector3::zero;

			//コントローラ振動を止める
			XboxInput(_Playeridx)->Vibration(0, 0);
			ChangeState(PStateE::STAY);
		}
	}
}

void Player::Move()
{
	//初期化
	_Move = Vector3::zero;
	if (!_IsAction)
	{
		const float speed = _Cparameter->Speed;
		_Dir = Vector3::zero;
		//コントローラー移動
		_Dir.x = (XboxInput(_Playeridx)->GetAnalog(AnalogInputE::L_STICK).x / 32767.0f)*speed;
		_Dir.z = (XboxInput(_Playeridx)->GetAnalog(AnalogInputE::L_STICK).y / 32767.0f)*speed;
#ifdef _DEBUG
		//キーボード(デバッグ用)
		if (KeyBoardInput->isPressed(DIK_W))
		{
			_Dir.z += speed;
		}
		if (KeyBoardInput->isPressed(DIK_S))
		{
			_Dir.z -= speed;
		}
		if (KeyBoardInput->isPressed(DIK_A))
		{
			_Dir.x -= speed;
		}
		if (KeyBoardInput->isPressed(DIK_D))
		{
			_Dir.x += speed;
		}
#endif // _DEBUG

		//移動したか
		if (_Dir.Length() != 0)
		{
			//ダッシュ
			if (XboxInput(_Playeridx)->IsPressButton(XINPUT_GAMEPAD_LEFT_SHOULDER)
#ifdef _DEBUG
				|| KeyBoardInput->isPressed(DIK_LSHIFT)
#endif // _DEBUG
				)
			{
				//はやくする
				_Dir.Scale(2.0f);
				if (_State != PStateE::FALL)
					ChangeState(PStateE::DASH);
			}
			else
			{
				//普通の歩き
				if (_State != PStateE::FALL)
					ChangeState(PStateE::WALK);
			}

			//カメラからみた向きに変換
			Camera* c = GameObjectManager::mainCamera;
			_Dir = c->transform->Direction(_Dir)*Time::DeltaTime();
			//Yの移動量を消す
			_Move = Vector3(_Dir.x, 0.0f, _Dir.z);

			Vector3 vec = _Move;
			//正規化
			vec.Normalize();
			//ベクトルから角度を求める
			float rot = D3DXToRadian(360) - atan2f(vec.z, vec.x);
			//回転
			transform->localAngle.y = D3DXToDegree(rot + D3DXToRadian(90));
		}
		//移動していない
		else if (_State == PStateE::WALK || _State == PStateE::DASH)
		{
			//待機
			ChangeState(PStateE::STAY);
		}
	}
	else
	{
		//滑る処理
		if (_JumpCount > 0)
			_Dir.Scale(0.97f);
		else
			_Dir.Scale(0.93f);
		_Move.Add(_Dir);
	}
}

void Player::Jump()
{
	//ジャンプ
	if (_IsAction == false &&
		(KeyBoardInput->isPush(DIK_J) || XboxInput(_Playeridx)->IsPushButton(XINPUT_GAMEPAD_Y)))
	{
		if (_JumpCount < 2)
		{
			_Jump = true;
			_Gravity = 0;
			_JumpCount++;
		}
	}
	if (_Jump)
	{
		float jumpPower = 350.0f * Time::DeltaTime();
		//上に移動
		_Move.y += jumpPower;
	}
}

void Player::Death()
{
	//とりあえずポジションだけで判定する
	if (transform->localPosition.y < -200)
	{
		//ストックを減らす？
		if (_Stock > 0)
		{
			_Stock--;
			//UIの数値更新
			_Pparameter->SetStock(_Stock);
		}

		//スコア更新
		_GameRule->UpdateScore(_LastAttack);

		_Reset();

		//残機が0なら(負数なら残機無限)
		if (_Stock == 0)
		{
			//死を伝える
			_GameRule->PlayerDeath(_Playeridx);
			this->SetActive(false);
			_IdxPlate->SetActive(false);
			_CrownPlate->SetActive(false);
		}
	}
}

#include "Item.h"

void Player::ItemAction()
{
	if (_HaveItem)
	{
		if (KeyBoardInput->isPush(DIK_X) || XboxInput(_Playeridx)->IsPushButton(XINPUT_GAMEPAD_RIGHT_SHOULDER))
		{
			//アイテムを離す
			_HaveItem->ToSeparate();
			_HaveItem = nullptr;
		}
	}
	//アイテムを持ってない
	else
	{
		//アイテム(ID:6)と当たっているか？
		const Collision *coll = (const Collision*)PhysicsWorld::Instance()->FindOverlappedDamageCollision(_RB, 6);
		//当たっている
		if (coll)
		{
			//ボタンが押された
			if (KeyBoardInput->isPush(DIK_Z) || XboxInput(_Playeridx)->IsPushButton(XINPUT_GAMEPAD_B))
			{
				//アイテムオブジェクト取得
				Item* item = (Item*)coll->gameObject;
				//アイテムを持つ
				if (item->ToHave(&_HandFrame->CombinedTransformationMatrix))
				{
					//持てたならアドレス保持
					_HaveItem = item;
				}
			}
		}
	}
}

void Player::SetInfo(CharacterInfo * info)
{
	SkinModelData* modeldata = new SkinModelData();
	modeldata->CloneModelData(SkinModelManager::LoadModel(info->Path), _Anim);
	_Pparameter->SetName(info->Name);
	_Model->SetModelData(modeldata);
	//持ち手のフレーム取得
	_HandFrame = (D3DXFRAME_DERIVED*)D3DXFrameFind(modeldata->GetFrameRoot(), "Item");
	//値コピー
	_Cparameter = info->Parameter;
}

void Player::AddKillCount()
{
	_KillCount++;
	_Pparameter->SetKill(_KillCount);
}

int Player::GetKillCount()
{
	return _KillCount;
}

int Player::GetStockCount()
{
	return _Stock;
}

void Player::No1(bool f)
{
	if (_Active == true)
		_CrownPlate->SetActive(f);
}

const Vector3 Player::GetDir()
{
	return _Dir;
}

void Player::_Reset()
{
	ChangeState(PStateE::STAY);
	_IsAction = false;
	_Jump = true;
	_JumpCount = 1;
	_Gravity = 0;
	_LastAttack = -1;
	_CharagePower = 0.0f;
	_Damage = 0;
	_Rigor = 0;
	_Pparameter->SetDamage(_Damage);
	_Blown = Vector3::zero;
	_Move = Vector3::zero;
	_Dir = Vector3::zero;

	//スタートポジションに移動(適当)
	transform->localPosition = Vector3(0, 200, 0);
	transform->localAngle = Vector3::zero;
}

void Player::_GravityCheck(const float & movey)
{
	//加速度
	const float addGravity = -9.8f * Time::DeltaTime();
	//重力加速
	_Gravity += addGravity;
	double MoveY = movey + _Gravity;
	//重力が発生している
	if (MoveY < -0.1f)
	{
		//地面との判定
		Vector3 start, end;
		//コリジョンの中心
		start = transform->position + Vector3(0, (_Height / 2), 0);
		//終点(落下先)
		end = start + Vector3(0, MoveY, 0);

		//ステージ(地面)とのあたり判定
		const SweepResultGround hit = PhysicsWorld::Instance()->FindOverlappedStage(_RB, start, end);
		//地面と当たった
		if (hit.isHit)
		{
			//ヒットした位置に着地
			transform->localPosition.y = hit.hitPos.y;
			_Jump = false;
			_JumpCount = 0;
			_Gravity = 0;
			if (!_IsAction)
			{
				if (_Dir.Length() != 0)
				{
					//ダッシュ
					if (XboxInput(_Playeridx)->IsPressButton(XINPUT_GAMEPAD_LEFT_SHOULDER)
#ifdef _DEBUG
						|| KeyBoardInput->isPressed(DIK_LSHIFT)
#endif // _DEBUG
						)
					{
						ChangeState(PStateE::DASH);
					}
					else
					{
						ChangeState(PStateE::WALK);
					}
				}
				else
				{
					ChangeState(PStateE::STAY);
				}
			}
		}
		else
		{
			//落下
			transform->localPosition.y += MoveY;
			//空中？
			if (!_IsAction)
				ChangeState(PStateE::FALL);
		}
	}
	else
	{
		//上昇
		transform->localPosition.y += MoveY;
	}
}
