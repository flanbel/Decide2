#include "Player.h"

#include "PlayerParameter.h"
#include "fbEngine\_Object\_GameObject\ParticleEmitter.h"
#include "fbEngine\_Object\_GameObject\Plate.h"
#include "fbEngine\_Object\_GameObject\AnimationPlate.h"
#include "fbEngine\_Object\_GameObject\SoundSource.h"
#include "fbEngine\_Object\_Component\_3D\Camera.h"

#include "Item.h"
#include "ItemList.h"
#include "GameRule.h"
#include "AttackCollision.h"

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
			Vector3(0,40,0),	//初速
			Vector2(30,30),		//サイズ
			1.0f,				//寿命
			0.02f,				//発生時間
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
			Color(1.5f,1.5f,1.0f,1.0f)	//乗算カラー
		}
	};

}

Player::Player(const char * name) :
	GameObject(name),
	_Damage(0),
	_Stock(1),
	_LastAttack(-1),
	_isJump(false),
	_IsAction(false),
	_IsAlive(false),
	_Gravity(0),
	_InterpolateTime(0.0f),
	_TransitionTime(0.0f)
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
	_Rigid->Create(1, coll, Collision_Attribute::PLAYER, Vector3::zero, Vector3(0, _Height / 2, 0));
	_RB = (btRigidBody*)_Rigid->GetCollisonObj();
	//RigidBodyの上下の移動量を消す
	_RB->setGravity(btVector3(0.0f, 0.0f, 0.0f));
	//物理ワールドで移動や回転をしなくてもスリープしないようにする。
	_RB->setSleepingThresholds(0.0f, 0.0f);
	//ゲームルール取得
	_GameRule = (GameRule*)INSTANCE(GameObjectManager)->FindObject("GameRule");
	//ストック数取得
	_Stock = _GameRule->GetStock();

	//パラメータ生成
	_Pparameter = INSTANCE(GameObjectManager)->AddNew<PlayerParameter>("_Pparameter", 0);
	_Pparameter->SetStock(_Stock);
	_Pparameter->SetDamage(_Damage);
	_Pparameter->SetDiscard(false);
	//プレート生成
	_IdxPlate = INSTANCE(GameObjectManager)->AddNew<Plate>("plate", 4);
	_IdxPlate->SetDiscard(false);
	//王冠生成
	_CrownPlate = INSTANCE(GameObjectManager)->AddNew<Plate>("Crown", 4);
	_CrownPlate->transform->SetParent(_IdxPlate->transform);
	_CrownPlate->transform->SetLocalPosition(Vector3(0, 60, 0));
	_CrownPlate->SetTexture(LOADTEXTURE("Crown.png"));
	_CrownPlate->SetDiscard(false);

	_PlayerColor = Color::white;

	//エフェクト

	_Aura = INSTANCE(GameObjectManager)->AddNew<AnimationPlate>("Aura", 4);
	_Aura->SetSplit(5, 2, 5 * 2);
	_Aura->SetTexture(LOADTEXTURE("Aura.jpg"));
	_Aura->SetSize(Vector2(128, 128));
	_Aura->SetBlendMode(fbEngine::BlendModeE::Add);
	_Aura->Play(1.0f);
	_Aura->SetBlendColor(Color::white * 1.3f);
	_Aura->SetPivot(0.5, 0.7);

	_Aura->SetDiscard(false);
	_Aura->SetActive(false);

	_Smoke = INSTANCE(GameObjectManager)->AddNew<ParticleEmitter>("SmokeEmit", 4);
	_Smoke->SetDiscard(false);
	_Smoke->transform->SetParent(transform);
	_Smoke->Init(parm[0]);
	_Smoke->SetEmitFlg(false);

	_Fire = INSTANCE(GameObjectManager)->AddNew<ParticleEmitter>("FireEmit", 4);
	_Fire->SetDiscard(false);
	_Fire->transform->SetParent(transform);
	_Fire->Init(parm[1]);
	_Fire->SetEmitFlg(false);
}

void Player::Start()
{
	float space = 300.0f;
	//パラメータを配置
	_Pparameter->transform->SetLocalPosition(Vector3(180.0f + (_Playeridx * space), 600.0f, 0.0f));
	//カメラ設定
	_Model->SetLight(INSTANCE(GameObjectManager)->mainLight);
	_Model->SetCamera(INSTANCE(GameObjectManager)->mainCamera);

	//プレイヤーの添え字設定。
	char path[10];
	Support::ToString(_Playeridx + 1, path);
	strcat_s(path, strlen(path) + strlen("P.png") + 1, "P.png");
	_IdxPlate->SetTexture(LOADTEXTURE(path));
	_IdxPlate->SetBlendColor(_PlayerColor);
	_IdxPlate->Start();

	//
	_CrownPlate->Start();
	_CrownPlate->SetBlendColor(Color::white * 3.0f);
	_CrownPlate->SetActive(false);

	_Pparameter->SetColor(_PlayerColor);
	//プレイヤーカラー設定。
	_Model->SetTextureBlend(_PlayerColor * 8.0f);
	
	//初期ポジ設定
	int idx = _Playeridx + 1;
	transform->SetPosition(Vector3(-200 + (idx % 2 * 400), 200,-200 + (idx / 3 * 400)));
	
	//いろいろなパラメータ初期化
	_Reset();
	_IsAlive = true;
}

void Player::Update()
{
	if (_IsAlive)
	{
		//開始前じゃないなら
		if (_GameRule->GetGameState() != GameRule::GameStateE::BEFORE_GAME)
		{
			_ItemAction();
			_Move();

			//ステートパターン実装したい。
			//TODO::アニメーションの機能に盛り込むとかして消したい。
			if (_IsAction)
			{
				//n割り再生したら遷移開始
				float normalTime;
				normalTime = 0.7f;
				if (_State == PStateE::KICK &&
					_Anim->GetTimeRatio() > normalTime)
				{
					//遷移時間設定
					_TransitionTime = _Anim->GetAnimationEndTime(_Anim->GetPlayAnimNo()) * (1.0f - normalTime);
					ChangeState(PStateE::STAY);
				}

				if (_State == PStateE::PUNCH &&
					_Anim->GetTimeRatio() > normalTime)
				{
					//遷移時間設定
					_TransitionTime = _Anim->GetAnimationEndTime(_Anim->GetPlayAnimNo()) * (1.0f - normalTime);
					ChangeState(PStateE::STAY);
				}

				normalTime = 0.8f;
				if (_State == PStateE::SLASH)
				{
					//遷移時間設定
					_TransitionTime = _Anim->GetAnimationEndTime(_Anim->GetPlayAnimNo()) * (1.0f - normalTime);
					//前に移動(適当)
					_Movement.Add(transform->Direction(Vector3(0, 0, 5.0f)));
					if (_Anim->GetTimeRatio() > normalTime)
					{
						ChangeState(PStateE::STAY);
					}
				}
			}

			_Attack();

			_Jump();
			_BlowOff();
		}

		//Y軸の移動量保存
		float moveY = _Movement.y;
		//Y軸移動はしない
		_Movement.y = 0.0f;
		if (_Movement.Length() > 0)
		{
			//XZ軸の移動
			transform->SetLocalPosition(transform->GetLocalPosition() + _Movement);
		}

		_GravityCheck(moveY);

		

		//場外にでたときの処理。
		_Death();

		//頭の上あたり
		Vector3 pos = transform->GetPosition();
		_Aura->transform->SetLocalPosition(pos + Vector3(0, 30, 0));
		_IdxPlate->transform->SetLocalPosition(pos + Vector3(0, 150, 0));
	}
	else
	{
		_RespawnTimer += Time::DeltaTime();
		if(_RespawnTimer > 3.0f)
		{
			//生き返る。
			_IsAlive = true;
			_Model->enable = true;
			_IdxPlate->SetActive(true);
			//スタートポジションに移動(適当)
			transform->SetPosition(Vector3(0, 200, 0));
		}
	}
}

void Player::ChangeState(PStateE next)
{
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
		_Aura->SetActive(false);
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
	_AnimationControl();
}

void Player::Damage(const int& idx, const float& damage, const Vector3& blow, const float& rigor)
{
	//吹き飛ばしベクトルと逆の向きを向くようにする。
	if(blow.Length() > 0.0f)
	{
		//吹き飛ぶ方向と反対のベクトル
		Vector2 vec = Vector2(blow.x * -1, blow.z * -1);
		//正規化
		vec.Normalize();
		Vector3 ang = transform->GetLocalAngle();
		//ベクトルから角度を求める
		ang.y = D3DXToDegree(vec.Rot());
		//回転
		transform->SetLocalAngle(ang);
	}

	//ダメージSE
	SoundSource* damageSE = INSTANCE(GameObjectManager)->AddNew<SoundSource>("DamageSE", 0);
	damageSE->Init("Asset/Sound/damage.wav");
	damageSE->SetDelete(true);
	damageSE->Play(false);

	//ダメージ加算(999が最大)
	_Damage = min(999, _Damage + damage);
	//テキストのダメージ更新
	_Pparameter->SetDamage(_Damage);

	//最後に攻撃してきたとしてやつ保存
	_LastAttack = idx;
	//タイマー初期化
	_RigorTimer = 0;
	//硬直時間受け取り
	_Rigor = rigor;
	//吹き飛び力受け取り
	_Blown = blow;
	//キャラクターの「吹き飛ばされやすさ」による吹き飛び補正
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

void Player::CheckInput()
{
	//Aボタン
	if (XboxInput(_Playeridx)->IsPushButton(XINPUT_GAMEPAD_A) || KeyBoardInput->isPush(DIK_SPACE))
	{
	
	}
	//離された。
	else
	{
	
	}

	//Bボタン
	if (XboxInput(_Playeridx)->IsPushButton(XINPUT_GAMEPAD_B) || KeyBoardInput->isPush(DIK_Z))
	{
	
	}

	//Yボタン
	if (XboxInput(_Playeridx)->IsPushButton(XINPUT_GAMEPAD_Y) || KeyBoardInput->isPush(DIK_J))
	{
	
	}

	//ライトショルダー
	if (XboxInput(_Playeridx)->IsPushButton(XINPUT_GAMEPAD_RIGHT_SHOULDER) || KeyBoardInput->isPush(DIK_X))
	{
	
	}

	//レフトショルダー
	if (XboxInput(_Playeridx)->IsPressButton(XINPUT_GAMEPAD_LEFT_SHOULDER) || KeyBoardInput->isPressed(DIK_LSHIFT))
	{

	}
}

void Player::_AnimationControl()
{
	switch (_State)
	{
	case PStateE::STAY:
		//待機モーション
		_PlayAnimation(0, 0.2f);
		_TransitionTime = 0.0f;
		break;
	case PStateE::WALK:
		_PlayAnimation(1, 0.2f);
		break;
	case PStateE::DASH:
		_PlayAnimation(2, 0.2f);
		break;
	case PStateE::JUMP:
		//NONE
		break;
	case PStateE::FALL:
		//降下中モーション
		_PlayAnimation(9, 0.2f);
		break;
	case PStateE::PUNCH:
		//パンチ再生
		_PlayAnimation(3, 0.2f, 1);
		break;
	case PStateE::KICK_CHARGE:
		//蹴りため開始
		_PlayAnimation(4, 0.2f, 1);
		break;
	case PStateE::KICK:
		_PlayAnimation(5, 0.1f, 1);
		break;
	case PStateE::SLASH:
		//武器振り再生
		_PlayAnimation(8, 0.2f, 1);
		break;
	case PStateE::DAMAGE:
		_PlayAnimation(6, 0.2f, 1);
		break;
	case PStateE::BLOW:
		_PlayAnimation(7, 0.2f, 1);
		break;
	default:
		break;
	}
}

void Player::_Attack()
{
	if (!_IsAction)
	{
		//Bボタン
		if ((KeyBoardInput->isPush(DIK_Z) || XboxInput(_Playeridx)->IsPushButton(XINPUT_GAMEPAD_B)))
		{
			//アイテムを持っていない
			if (!_HaveItem)
			{
				//パンチ	
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
			//_Fire->SetEmitFlg(true);
			_Aura->SetActive(true);
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

		//攻撃のあたり判定
		AttackCollision* obj;
		//攻撃のあたり判定の情報
		DamageCollision::DamageCollisonInfo Dinfo;

		//移動量が吹き飛び値の補正になる
		Vector3 V = _Dir;
		V.Scale(0.4f);

		//パンチの攻撃判定
		if (_State == PStateE::PUNCH &&
			nowFrame == 8)
		{
			//素振りSE
			SoundSource* swingSE = INSTANCE(GameObjectManager)->AddNew<SoundSource>("SwingSE", 0);
			swingSE->Init("Asset/Sound/swing.wav");
			swingSE->SetDelete(true);
			swingSE->Play(false);

			//あたり判定を出す
			obj = INSTANCE(GameObjectManager)->AddNew<AttackCollision>("panch", 1);
			//球状のあたり判定
			SphereCollider* sphere = obj->AddComponent<SphereCollider>();
			sphere->Create(30);

			//移動
			obj->transform->SetParent(this->transform);
			obj->transform->SetLocalPosition(Vector3(0, 60, 40));

			//攻撃力補正
			Dinfo.Damage = Random::Range(2, 3) * _Cparameter->Power;
			Dinfo.Blown = transform->Direction(Vector3(0.0f, 60.0f, 60.0f) + V);
			//吹き飛ばし補正
			Dinfo.Blown.Scale(_Cparameter->BlowCorrection);
			Dinfo.Rigor = 0.2f;
			Dinfo.StopTime = 5;
			float life = 0.2f;
			obj->Create(sphere, _Playeridx, Collision_Attribute::DAMAGE, life, Dinfo);
		}

		//キックの攻撃判定
		if (_State == PStateE::KICK &&
			nowFrame == 15)
		{
			//素振りSE
			SoundSource* swingSE = INSTANCE(GameObjectManager)->AddNew<SoundSource>("SwingSE", 0);
			swingSE->Init("Asset/Sound/swing.wav");
			swingSE->SetDelete(true);
			swingSE->Play(false);

			//あたり判定を出す
			obj = INSTANCE(GameObjectManager)->AddNew<AttackCollision>("kick", 1);
			//球状のあたり判定
			SphereCollider* sphere = obj->AddComponent<SphereCollider>();
			sphere->Create(40);

			//移動
			obj->transform->SetParent(this->transform);
			obj->transform->SetLocalPosition(Vector3(0, 60, 50));

			//攻撃力補正
			Dinfo.Damage = (Random::Range(7, 10) * (_CharagePower + 0.2f)) * _Cparameter->Power;
			Dinfo.Blown = transform->Direction(Vector3(0.0f, 120.0f, 180.0f) + V);
			//吹き飛ばし補正
			Dinfo.Blown.Scale(_Cparameter->BlowCorrection * (_CharagePower + 0.5f));
			Dinfo.Rigor = 0.6f;
			Dinfo.StopTime = 10;
			float life = 0.2f;
			obj->Create(sphere, _Playeridx, Collision_Attribute::DAMAGE, life, Dinfo);
		}

		//剣の攻撃判定
		if (_State == PStateE::SLASH &&
			nowFrame <= 30 && (nowFrame % 5) == 0)
		{
			//素振りSE
			SoundSource* swingSE = INSTANCE(GameObjectManager)->AddNew<SoundSource>("SwingSE", 0);
			swingSE->Init("Asset/Sound/swing2.wav");
			swingSE->SetDelete(true);
			swingSE->Play(false);

			//あたり判定を出す
			obj = INSTANCE(GameObjectManager)->AddNew<AttackCollision>("sword", 1);
			//球状のあたり判定
			SphereCollider* sphere = obj->AddComponent<SphereCollider>();
			sphere->Create(35);
			//移動
			obj->transform->SetParent(this->transform);
			obj->transform->SetLocalPosition(Vector3(0, 60, 30));

			//攻撃力補正
			Dinfo.Damage = Random::Range(1, 2) * _Cparameter->Power;
			Dinfo.Blown = transform->Direction(Vector3(60.0f, 90.0f, 0.0f) + V);
			//吹き飛ばし補正
			Dinfo.Blown.Scale(_Cparameter->BlowCorrection);
			Dinfo.Rigor = 0.1f;
			Dinfo.StopTime = 1;
			float life = 0.05f;
			obj->Create(sphere, _Playeridx, Collision_Attribute::DAMAGE, life, Dinfo);
		}
	}
}

void Player::_ItemAction()
{
	//アイテムを持っている
	if (_HaveItem)
	{
		//Bボタン
		if ((KeyBoardInput->isPush(DIK_Z) || XboxInput(_Playeridx)->IsPushButton(XINPUT_GAMEPAD_B)))
		{
			switch (_HaveItem->GetItemType())
			{
			case ItemInfo::ItemTypeE::Throw:
				//アイテムを離す
				_HaveItem->ToSeparate(transform->GetForward() * 10.0f + _Movement * 2.0f, _Playeridx);
				_HaveItem = nullptr;
				break;
			case ItemInfo::ItemTypeE::Slash:
				ChangeState(PStateE::SLASH);
				break;
			default:
				break;
			}
		}

		//捨てる
		if (KeyBoardInput->isPush(DIK_X) || XboxInput(_Playeridx)->IsPushButton(XINPUT_GAMEPAD_RIGHT_SHOULDER))
		{
			//アイテムを離す
			_HaveItem->ToSeparate(_Movement * 2.0f, _Playeridx);
			_HaveItem = nullptr;
		}
	}
	//アイテムを持ってない
	else
	{
		//ボタンが押された
		if (KeyBoardInput->isPush(DIK_Z) || XboxInput(_Playeridx)->IsPushButton(XINPUT_GAMEPAD_B))
		{
			//アイテムと当たっているか？
			const Collision *coll = (const Collision*)INSTANCE(PhysicsWorld)->ClosestContactTest(_Rigid, Collision_Attribute::ITEM);
			//当たっている
			if (coll)
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

void Player::_Move()
{
	//初期化
	_Movement = Vector3::zero;
	if (!_IsAction)
	{
		float speed = _Cparameter->Speed;
		_Dir = Vector3::zero;
		//コントローラー移動
		_Dir.x = (XboxInput(_Playeridx)->GetAnalog(AnalogInputE::L_STICK).x / 32767.0f);
		_Dir.z = (XboxInput(_Playeridx)->GetAnalog(AnalogInputE::L_STICK).y / 32767.0f);
#ifdef _DEBUG
		//キーボード(デバッグ用)
		if (KeyBoardInput->isPressed(DIK_W))
		{
			_Dir.z += 1;
		}
		if (KeyBoardInput->isPressed(DIK_S))
		{
			_Dir.z -= 1;
		}
		if (KeyBoardInput->isPressed(DIK_A))
		{
			_Dir.x -= 1;
		}
		if (KeyBoardInput->isPressed(DIK_D))
		{
			_Dir.x += 1;
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
				speed *= 2.0f;
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
			Camera* c = INSTANCE(GameObjectManager)->mainCamera;
			_Dir = c->transform->Direction(_Dir) * speed * Time::DeltaTime();
			//Yの移動量を消す
			_Dir.y = 0;
			_Movement = _Dir;
			_Movement.Normalize();
			_Movement.Scale(speed * Time::DeltaTime());

			//移動方向
			Vector2 vec = Vector2(_Dir.x, _Dir.z);
			//正規化
			vec.Normalize();
			Vector3 ang = transform->GetLocalAngle();
			//y軸回転
			ang.y = D3DXToDegree(vec.Rot());
			//回転
			transform->SetLocalAngle(ang);
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
		_Movement.Add(_Dir);
	}
}

void Player::_Jump()
{
	//ジャンプ
	if (_IsAction == false &&
		(KeyBoardInput->isPush(DIK_J) || XboxInput(_Playeridx)->IsPushButton(XINPUT_GAMEPAD_Y)))
	{
		if (_JumpCount < 2)
		{
			_isJump = true;
			_Gravity = 0;
			_JumpCount++;
		}
	}
	if (_isJump)
	{
		float jumpPower = 350.0f * Time::DeltaTime();
		//上に移動
		_Movement.y += jumpPower;
	}
}

void Player::_BlowOff()
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
			_Movement.Add(_Blown * delta);
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
			_Movement.Add(_Blown);
			//吹き飛ばしを0に
			_Blown = Vector3::zero;

			//コントローラ振動を止める
			XboxInput(_Playeridx)->Vibration(0, 0);
			ChangeState(PStateE::STAY);
		}
	}
}

void Player::_Death()
{
	//とりあえずポジションだけで判定する
	if (transform->GetPosition().y < -200)
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
		
		{
			_IsAlive = false;
			_Model->enable = false;
			_IdxPlate->SetActive(false);
		}


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

void Player::_PlayAnimation(int idx, float time, int loop)
{
	//今と次のアニメーションが異なる時のみ変更
	if (_Anim->GetPlayAnimNo() != idx)
	{
		_Anim->PlayAnimation(idx, time, _TransitionTime, loop);
	}
}

void Player::_Reset()
{
	ChangeState(PStateE::STAY);
	_IsAction = false;
	_isJump = true;
	_JumpCount = 1;
	_Gravity = 0;
	_LastAttack = -1;
	_CharagePower = 0.0f;
	_Damage = 0;
	_Rigor = 0;
	_RespawnTimer = 0;
	_Pparameter->SetDamage(_Damage);
	_Blown = Vector3::zero;
	_Movement = Vector3::zero;
	_Dir = Vector3::zero;

	transform->SetLocalAngle(Vector3::zero);
}

void Player::_GravityCheck(const float & movey)
{
	//加速度
	const float addGravity = -9.8f * Time::DeltaTime();
	//重力加速
	_Gravity += addGravity;
	double MoveY = movey + _Gravity;
	//重力が発生している
	if (MoveY < 0.0f)
	{
		//始点と終点
		Vector3 start, end;
		//コリジョンの中心
		start = transform->GetPosition() + Vector3(0, (_Height / 2), 0);
		//終点(落下先)
		end = start + Vector3(0, MoveY, 0);

		//ステージ(地面)とのあたり判定
		const fbPhysicsCallback::SweepResultGround hit = INSTANCE(PhysicsWorld)->FindOverlappedStage(_RB, start, end);
		//地面と当たった
		if (hit.isHit)
		{
			//ヒットした位置に着地
			Vector3 pos = transform->GetPosition();
			pos.y = hit.hitPos.y;
			transform->SetPosition(pos);
			//ジャンプ終了
			_isJump = false;
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
			Vector3 lpos = transform->GetLocalPosition();
			lpos.y += MoveY;
			transform->SetLocalPosition(lpos);
			//空中？
			if (!_IsAction)
				ChangeState(PStateE::FALL);
		}
	}
	else
	{
		//上昇
		Vector3 lpos = transform->GetLocalPosition();
		lpos.y += MoveY;
		transform->SetLocalPosition(lpos);
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
