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

Player::Player(char * name) :
	GameObject(name),
	damage(0),
	stock(1),
	lastAttack(-1),
	jump(false),
	isAction(false)
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
	model = AddComponent<SkinModel>();
	rigid = AddComponent<RigidBody>();
	BoxCollider* coll = AddComponent<BoxCollider>();
	anim = AddComponent<Animation>();

	coll->Create(Vector3(25, 100, 25));
	rigid->Create(1, coll, 5, Vector3::zero, Vector3(0, 50, 0));
	rb = (btRigidBody*)rigid->GetCollisonObj();
	//スリープさせない
	rb->setSleepingThresholds(0, 0);
	//ゲームルール取得
	gameRule = (GameRule*)GameObjectManager::FindObject("GameRule");
	//ストック数取得
	stock = gameRule->GetStock();

	//パラメータ生成
	Pparameter = GameObjectManager::AddNew<PlayerParameter>("Pparameter", 0);
	Pparameter->SetStock(stock);
	Pparameter->SetDamage(damage);
	Pparameter->Discard(false);
	//プレート生成
	idxPlate = GameObjectManager::AddNew<PlatePrimitive>("plate", 4);
	idxPlate->Discard(false);

	isAction = false;
	jumpCount = 1;
	blown = Vector3::zero;
	ChangeState(PState::STAY);
	CharagePower = 0.0f;
	playerColor = Color::white;
	//初期ポジ(適当)
	transform->localPosition.y = 200;


	ParicleParameter parm[2] =
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
		Color(1.5f,1.5f,1.5f,1.0f)	//乗算カラー
	}
	};
	smoke = GameObjectManager::AddNew<ParticleEmitter>("SmokeEmit", 2);
	smoke->Discard(false);
	smoke->transform->SetParent(transform);
	smoke->Init(parm[0]);
	smoke->Emit(false);

	fire = GameObjectManager::AddNew<ParticleEmitter>("FireEmit", 2);
	fire->Discard(false);
	fire->transform->SetParent(transform);
	fire->Init(parm[1]);
	fire->Emit(false);
}

void Player::Start()
{
	//パンチの効果音を鳴らすオブジェクト
	//オブジェクトっておかしくね？
	PunchSound = GameObjectManager::AddNew<SoundSource>("PunchSound", 0);
	PunchSound->Init("Asset/Sound/punch.wav");

	Pparameter->transform->localPosition = Vector3(100 +(playeridx) * 300, 550, 0);
	model->SetCamera(GameObjectManager::mainCamera);
	model->SetLight(GameObjectManager::mainLight);

	char path[10];
	InttoString(playeridx + 1, path);
	strcat_s(path, strlen(path) + strlen("P.png") + 1, "P.png");
	idxPlate->SetTexture(TextureManager::LoadTexture(path));
	idxPlate->SetBlendColor(playerColor);
	Pparameter->SetColor(playerColor);
	model->SetTextureBlend(playerColor);
}

#include "DamageCollision.h"
#include "AttackCollision.h"
#include "fbEngine/Camera.h"
void Player::Update()
{
	UpdateStateMachine();
	//n割り再生したら遷移開始
	if (state == PState::KICK &&
		anim->GetTimeRatio() > 0.7f)
	{
		ChangeState(PState::STAY);
	}

	if (state == PState::PUNCH &&
		anim->GetTimeRatio() > 0.7f)
	{
		ChangeState(PState::STAY);
	}

	if (state == PState::SLASH &&
		anim->GetTimeRatio() > 0.8f)
	{
		ChangeState(PState::STAY);
	}

	ItemAction();
	move = Vector3::zero;
	if (!isAction)
	{
		Move();
	}else
	{
		//滑る？
		if (jumpCount > 0)
			dir.Scale(0.97f);
		else
			dir.Scale(0.93f);
		move.Add(dir);
	}

	Attack();
	Damage();

	Jump();
	Blown();

	//移動
	if (move.Length() > 0)
	{
		transform->localPosition.Add(move);
		transform->UpdateTransform();
		rigid->Update();
	}
	
	//重力が発生している
	if(jumpCount > 0)
	{
		//地面との判定
		Vector3 start, end;
		//コリジョンの中心
		start = transform->position + Vector3(0, 50, 0);
		
		btVector3 v = rb->getLinearVelocity() * Time::DeltaTime();
		if ((move.y + v.y()) <= 0)
			//{
			//	if (v.y() > 0)
			//	{
			//		//上昇中
			//		end = transform->position - Vector3(0, v.y()*0.01f, 0);
			//	}
			//	else
			//	{
			//		end = transform->position - Vector3(0, 0.0001f, 0);
			//	}
			//}
			//else
		{
			//下降中
			if (v.y() > 0)
			{
				end = transform->position + Vector3(0, v.y(), 0);
			}
			else
			{
				end = transform->position + Vector3(0, v.y() + 0.01f, 0);
			}

			if (!isAction)
				ChangeState(PState::FALL);


			//ステージ(地面)とのあたり判定
			const bool isHit = (const DamageCollision*)PhysicsWorld::Instance()->FindOverlappedStage(rb, start, end);
			//着地した
			if (isHit)
			{
				jump = false;
				jumpCount = 0;
				if (!isAction)
				{
					if (dir.Length() != 0)
					{
						//ダッシュ
						if (XboxInput(playeridx)->IsPressButton(XINPUT_GAMEPAD_LEFT_SHOULDER)
#ifdef _DEBUG
							|| KeyBoardInput->isPressed(DIK_LSHIFT)
#endif // _DEBUG
							)
						{
							ChangeState(PState::DASH);
						}
						else
						{
							ChangeState(PState::WALK);
						}
					}
					else
					{
						ChangeState(PState::STAY);
					}
				}
			}
		}
	}

	AnimationControl();

	

	//場外にでたときの処理。
	Death();

}

void Player::LateUpdate()
{
	idxPlate->transform->localPosition = transform->LocalPos(Vector3(-50, 150, 0));
	idxPlate->transform->Update();
	idxPlate->transform->LockAt(GameObjectManager::mainCamera->gameObject);
}

void Player::UpdateStateMachine()
{
	////nullチェック
	//if(currentState != nullptr)
	//{
	//	currentState->Update();
	//}
}

void Player::ChangeState(PState next)
{
	//if(currentState != nullptr)
	//{
	//	//ステートを抜ける
	//	currentState->Leave();
	//}
	//switch (next)
	//{
	//case PState::STAY:
	//	currentState = &StayState;
	//	break;
	//case PState::WALK:
	//	currentState = &WalkState;
	//	break;
	//case PState::DASH:
	//	currentState = &RunState;
	//	break;
	//case PState::JUMP:
	//	currentState = &JumpState;
	//	break;
	//case PState::FALL:
	//	currentState = &FallState;
	//	break;
	//case PState::PUNCH:
	//	currentState = &StayState;
	//	break;
	//case PState::KICK_CHARGE:
	//	currentState = &StayState;
	//	break;
	//case PState::KICK:
	//	currentState = &StayState;
	//	break;
	//case PState::SLASH:
	//	currentState = &StayState;
	//	break;
	//case PState::DAMAGE:
	//	currentState = &StayState;
	//	break;
	//case PState::BLOW:
	//	currentState = &StayState;
	//	break;
	//default:
	//	//エラー
	//	break;
	//}
	//state = next;
	//currentState->Enter();

	//今のステートを抜けるときの処理
	switch (state)
	{
	case PState::STAY:
		break;
	case PState::WALK:
		break;
	case PState::DASH:
		smoke->Emit(false);
		break;
	case PState::JUMP:
		break;
	case PState::FALL:
		break;
	case PState::PUNCH:
		isAction = false;
		break;
	case PState::KICK_CHARGE:
		isAction = false;
		fire->Emit(false);
		break;
	case PState::KICK:
		isAction = false;
		break;
	case PState::SLASH:
		isAction = false;
		break;
	case PState::DAMAGE:
		break;
	case PState::BLOW:
		smoke->Emit(false);
		break;
	default:
		break;
	}
	//次のステートに切り替わるときの処理
	switch (next)
	{
	case PState::STAY:
		break;
	case PState::WALK:
		break;
	case PState::DASH:
		smoke->Emit(true);
		break;
	case PState::JUMP:
		break;
	case PState::FALL:
		break;
	case PState::PUNCH:
		isAction = true;
		break;
	case PState::KICK_CHARGE:
		isAction = true;
		break;
	case PState::KICK:
		isAction = true;
		break;
	case PState::SLASH:
		isAction = true;
		break;
	case PState::DAMAGE:
		break;
	case PState::BLOW:
		smoke->Emit(true);
		break;
	default:
		break;
	}
	state = next;
}

void Player::PlayAnimation(int idx, float time, int loop)
{
	//異なるアニメーションの時のみ再生
	if(anim->GetPlayAnimNo() != idx)
	{
		anim->PlayAnimation(idx, time, loop);
	}
}

void Player::AnimationControl()
{
	switch (state)
	{
	case PState::STAY:
		//待機モーション
		PlayAnimation(0, 0.2f);
		break;
	case PState::WALK:
		PlayAnimation(1, 0.2f);
		break;
	case PState::DASH:
		PlayAnimation(2, 0.2f);
		break;
	case PState::JUMP:
		//NONE
		break;
	case PState::FALL:
		//降下中モーション
		PlayAnimation(9, 0.2f);
		break;
	case PState::PUNCH:
		//パンチ再生
		PlayAnimation(3, 0.2f, 1);
		anim->SetAnimeSpeed(2.0f);
		break;
	case PState::KICK_CHARGE:
		//蹴りため開始
		PlayAnimation(4, 0.2f, 1);
		break;
	case PState::KICK:
		PlayAnimation(5, 0.1f, 1);
		break;
	case PState::SLASH:
		//武器振り再生
		PlayAnimation(8, 0.2f, 1);
		anim->SetAnimeSpeed(1.5f);
		break;
	case PState::DAMAGE:
		PlayAnimation(6, 0.2f, 1);
		break;
	case PState::BLOW:
		PlayAnimation(7, 0.2f, 1);
		break;
	default:
		break;
	}
}

void Player::Attack()
{
	if (!isAction)
	{
		//Bボタン
		if ((KeyBoardInput->isPush(DIK_Z) || XboxInput(playeridx)->isPushButton(XINPUT_GAMEPAD_B)))
		{
			//アイテムを持っている
			if (haveItem)
			{
				ChangeState(PState::SLASH);
			}
			//パンチ
			else
			{
				ChangeState(PState::PUNCH);
			}
		}

		//Aボタン
		if ((KeyBoardInput->isPush(DIK_SPACE) || XboxInput(playeridx)->isPushButton(XINPUT_GAMEPAD_A)))
		{
			ChangeState(PState::KICK_CHARGE);
		}
	}

	//チャージ中
	if (state == PState::KICK_CHARGE)
	{
		//フルチャージ
		if (anim->GetPlaying() == false)
		{
			fire->Emit(true);
		}
		//ボタンが離された
		if (!KeyBoardInput->isPressed(DIK_SPACE) && !XboxInput(playeridx)->IsPressButton(XINPUT_GAMEPAD_A))
		{
			float rate = anim->GetTimeRatio();
			//フルチャージなら1.5倍
			CharagePower = (rate >= 1.0f) ? 1.5f : rate;
			ChangeState(PState::KICK);
		}
	}


	if (isAction)
	{
		//現在のフレーム取得
		int frame = anim->NowFrame();

		//移動量が吹き飛び値の補正になる
		Vector3 V = dir;
		V.Scale(0.3f);

		//パンチの攻撃判定
		if (state == PState::PUNCH &&
			frame == 8)
		{
			PunchSound->Play(false);
			//あたり判定を出す
			AttackCollision* obj = GameObjectManager::AddNew<AttackCollision>("panch", 1);
			Transform* t = obj->GetComponent<Transform>();
			//前に出す
			t->localPosition = transform->Local(Vector3(0, 60, 30));
			t->localAngle = transform->localAngle;
			t->UpdateTransform();

			DamageCollision::DamageCollisonInfo info;
			//攻撃力補正
			info.damage = Random::Range(2, 3) * Cparameter.power;
			info.blown = transform->Direction(Vector3(0.0f, 0.5f, 2.0f) + V);
			//吹き飛ばし補正
			info.blown.Scale(Cparameter.blowCorrection);
			info.rigor = 0.2f;
			info.stoptime = 5;
			obj->Initialize(playeridx, 0.2f, Vector3(50, 50, 50), info);
		}

		//キックの攻撃判定
		if (state == PState::KICK &&
			frame == 15)
		{
			PunchSound->Play(false);
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
			info.damage = (Random::Range(7, 10) * (CharagePower + 0.2f)) * Cparameter.power;
			info.blown = transform->Direction(Vector3(0.0f, 2.0f, 3.0f) + V);
			//吹き飛ばし補正
			info.blown.Scale(Cparameter.blowCorrection * (CharagePower + 0.5f));
			info.rigor = 0.6f;
			info.stoptime = 10;
			obj->Initialize(playeridx, 0.2f, size, info);
		}

		//剣の攻撃判定
		if (state == PState::SLASH &&
			frame == 7)
		{
			//あたり判定を出す
			AttackCollision* obj = GameObjectManager::AddNew<AttackCollision>("sword", 1);
			Transform* t = obj->GetComponent<Transform>();
			//前に出す
			t->localPosition = transform->Local(Vector3(0, 60, 30));
			t->localAngle = transform->localAngle;
			t->UpdateTransform();
			Vector3 size = Vector3(40, 70, 40);

			DamageCollision::DamageCollisonInfo info;
			//攻撃力補正
			info.damage = Random::Range(10, 20) * Cparameter.power;
			info.blown = transform->Direction(Vector3(0.0f, 0.1f, 0.1f) + V);
			//吹き飛ばし補正
			info.blown.Scale(Cparameter.blowCorrection);
			info.rigor = 0.1f;
			info.stoptime = 1;
			obj->Initialize(playeridx, 0.1f, size, info);
		}
	}
}

void Player::Damage()
{
	//ダメージ処理
	FOR(PLAYER_NUM)
	{
		//自分のあたり判定はしない
		if (i == playeridx)
			continue;

		const DamageCollision *co = (const DamageCollision*)PhysicsWorld::Instance()->FindOverlappedDamageCollision(rb, i);
		if (co)
		{
			//吹き飛ぶ方向
			Vector3 vec = co->info.blown;
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
			lastAttack = i;
			//ダメージ加算(999が最大)
			damage = min(999, damage + co->info.damage);
			//テキストのダメージ更新
			Pparameter->SetDamage(damage);
			//タイマー初期化
			rigortimer = 0;
			//硬直時間受け取り
			rigor = co->info.rigor;
			//吹き飛び力受け取り
			blown = co->info.blown;
			//吹き飛び補正
			blown.Scale(Cparameter.toBlowCorrection);
			//更にダメージによる吹き飛び補正
			blown.Scale(1.0f + (float)damage / 30.0f);

			//重力を0に
			btVector3 v = rb->getLinearVelocity();
			//下方向のやつを消す
			rb->setLinearVelocity(btVector3(v.x(), max(0.0f, v.y()), v.z()));
			//吹き飛ばし力があるなら
			if (blown.Length() > 0)
			{
				//コントローラ振動
				XboxInput(playeridx)->Vibration(20000, 20000);
				//吹き飛んだのでジャンプ回数を1に
				jumpCount = 1;
			}
		}
	}

}

void Player::Blown()
{
	//吹き飛ばし力があるのなら。
	if (blown.Length() > 0)
	{
		//前フレームとの時間の差
		float delta = Time::DeltaTime();
		//ぶっ飛び計算
		rigortimer += delta;
		//硬直時間以下
		if (rigortimer <= rigor)
		{
			//吹き飛び
			move.Add(blown);
			ChangeState(PState::BLOW);
		}
		else
		{
			//超過した時間
			float overtime = rigortimer - rigor;
			//失われし時
			float losttime = delta - overtime;
			//吹き飛ばし
			blown.Scale(losttime);
			move.Add(blown);
			//吹き飛ばしを0に
			blown = Vector3::zero;

			//コントローラ振動を止める
			XboxInput(playeridx)->Vibration(0, 0);
			ChangeState(PState::STAY);
		}
	}
}

void Player::Move()
{
	float s = Cparameter.speed;
	dir = Vector3::zero;
	//コントローラー移動
	dir.x = (XboxInput(playeridx)->GetAnalog(AnalogInput::L_STICK).x / 32767.0f)*s;
	dir.z = (XboxInput(playeridx)->GetAnalog(AnalogInput::L_STICK).y / 32767.0f)*s;
#ifdef _DEBUG
	//キーボード(デバッグ用)
	if (KeyBoardInput->isPressed(DIK_W))
	{
		dir.z += s;
	}
	if (KeyBoardInput->isPressed(DIK_S))
	{
		dir.z -= s;
	}
	if (KeyBoardInput->isPressed(DIK_A))
	{
		dir.x -= s;
	}
	if (KeyBoardInput->isPressed(DIK_D))
	{
		dir.x += s;
	}
#endif // _DEBUG

	//移動したか
	if (dir.Length() != 0)
	{
		//ダッシュ
		if (XboxInput(playeridx)->IsPressButton(XINPUT_GAMEPAD_LEFT_SHOULDER)
#ifdef _DEBUG
			|| KeyBoardInput->isPressed(DIK_LSHIFT)
#endif // _DEBUG
			)
		{
			//はやくする
			dir.Scale(2.0f);
			if (state != PState::FALL)
				ChangeState(PState::DASH);
		}
		else
		{
			//普通の歩き
			if (state != PState::FALL)
				ChangeState(PState::WALK);
		}

		//カメラからみた向きに変換
		Camera* c = GameObjectManager::mainCamera;
		dir = c->transform->Direction(dir);
		//Yの移動量を消す
		dir.y = 0.0f;
		move = Vector3(dir.x, dir.y, dir.z);

		Vector3 vec = dir;
		//正規化
		vec.Normalize();
		//ベクトルから角度を求める
		float rot = D3DXToRadian(360) - atan2f(vec.z, vec.x);
		//回転
		transform->localAngle.y = D3DXToDegree(rot + D3DXToRadian(90));
	}
	//移動していない
	else if(state == PState::WALK || state == PState::DASH)
	{
		//待機
		ChangeState(PState::STAY);
	}
}

void Player::Jump()
{
	//ジャンプ
	if (isAction == false &&
		(KeyBoardInput->isPush(DIK_J) || XboxInput(playeridx)->isPushButton(XINPUT_GAMEPAD_Y)))
	{
		if (jumpCount < 2)
		{
			jump = true;
			btVector3 v = rb->getLinearVelocity();
			//下方向のやつを消す
			rb->setLinearVelocity(btVector3(v.x(), max(0.0f, v.y()), v.z()));
			jumpCount++;
		}
	}
	if (jump)
	{
		float jumpPower = 9.0f;
		//上に移動
		move.y += jumpPower;
	}
}

void Player::Death()
{
	//とりあえずポジションだけで判定する
	if (transform->localPosition.y < -200)
	{
		//ストックを減らす？
		if (stock > 0)
		{
			stock--;
			//UIの数値更新
			Pparameter->SetStock(stock);
		}

		//最後に攻撃してきたやつのキル数増加
		if (lastAttack >= 0)
			gameRule->AddKillCount(lastAttack);

		lastAttack = -1;
		damage = 0;
		Pparameter->SetDamage(damage);
		blown = Vector3::zero;
		rigor = 0;
		jumpCount = 1;
		jump = true;
		btVector3 v = rb->getLinearVelocity();
		//下方向のやつを消す
		rb->setLinearVelocity(btVector3(v.x(), max(0.0f, v.y()), v.z()));

		//残機が0じゃないなら(負数なら残機無限)
		if (stock != 0)
		{
			//スタートポジションに移動
			transform->localPosition = Vector3(0, 200, 0);
			transform->localAngle = Vector3::zero;
		}
		else
		{
			//死
			//死を伝える
			gameRule->PlayerDeath(playeridx);
			this->SetActive(false);
			idxPlate->SetActive(false);
		}
	}
}

#include "Item.h"

void Player::ItemAction()
{
	if(haveItem)
	{
		//離す
		if (KeyBoardInput->isPush(DIK_X) || XboxInput(playeridx)->isPushButton(XINPUT_GAMEPAD_RIGHT_SHOULDER))
		{
			haveItem->ToSeparate();
			haveItem = nullptr;
		}
	}
	//アイテムを持ってない
	else
	{
		//アイテム(ID:6)と当たっているか？
		const Collision *coll = (const Collision*)PhysicsWorld::Instance()->FindOverlappedDamageCollision(rb, 6);
		//当たっている
		if (coll)
		{
			if (KeyBoardInput->isPush(DIK_Z) || XboxInput(playeridx)->isPushButton(XINPUT_GAMEPAD_B))
			{
				haveItem = (Item*)coll->gameObject;
				//アイテムを持つ
				haveItem->ToHave(&handFrame->CombinedTransformationMatrix);
			}
		}
	}
}

void Player::SetInfo(CharacterInfo * info)
{
	SkinModelData* modeldata = new SkinModelData();
	modeldata->CloneModelData(SkinModelManager::LoadModel(info->path), anim);
	Pparameter->SetName(info->name);
	model->SetModelData(modeldata);
	//持ち手のフレーム取得
	handFrame = (D3DXFRAME_DERIVED*)D3DXFrameFind(modeldata->GetFrameRoot(), "Item");
	//値コピー
	Cparameter = info->parameter;
}

void Player::AddKillCount()
{
	killCount++;
	Pparameter->SetKill(killCount);
}

int Player::GetKillCount()
{
	return killCount;
}

const Vector3 Player::GetDir()
{
	return dir;
}