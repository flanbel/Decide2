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

void Player::Awake()
{
	model = AddComponent<SkinModel>();
	rigid = AddComponent<RigidBody>();
	BoxCollider* coll = AddComponent<BoxCollider>();
	anim = AddComponent<Animation>();

	coll->Create(Vector3(15,50,15));
	rigid->Create(1, coll,5, Vector3::zero, Vector3(0, 25, 0));
	rb = (btRigidBody*)rigid->GetCollisonObj();
	//スリープさせない
	rb->setSleepingThresholds(0, 0);

	transform->localPosition.y = 150;

	damage = 0;
	stock = 1;
	Pparameter = GameObjectManager::AddNew<PlayerParameter>("Pparameter", 0);
	Pparameter->SetStock(stock);
	Pparameter->SetDamage(damage);
	Pparameter->Discard(false);

	idxPlate = GameObjectManager::AddNew<PlatePrimitive>("plate", 4);
	idxPlate->Discard(false);
	
	blown = Vector3::zero;
	state = PlayerState::Stay;
	old = (PlayerState)-1;
	isAction = false;
	CharagePower = 0.0f;
	playerColor = Color::white;

	PunchSound = GameObjectManager::AddNew<SoundSource>("PunchSound", 0);
	//PunchSound->Init("Asset/Sound/punch.wav");
	

	ParicleParameter parm =
	{
		"SmokeBall.png",	//パス
		Vector3(0,0,0),		//初速
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
	};
	smoke = GameObjectManager::AddNew<ParticleEmitter>("Emit", 4);
	smoke->Discard(false);
	smoke->transform->SetParent(transform);
	smoke->Init(parm);
	smoke->Emit(false);
}

void Player::Start()
{
	Pparameter->transform->localPosition = Vector3((playeridx + 1) * 250, 550, 0);
	model->SetCamera(GameObjectManager::mainCamera);
	model->SetLight(GameObjectManager::mainLight);

	char path[10];
	InttoString(playeridx + 1, path);
	strcat_s(path, strlen(path) + strlen("P.png") + 1, "P.png");
	idxPlate->SetTexture(TextureManager::LoadTexture(path));
	idxPlate->SetBlendColor(playerColor);

	jump = false;
}

#include "DamageCollision.h"
#include "AttackCollision.h"
#include "fbEngine/Camera.h"
void Player::Update()
{
	//攻撃中ではないなら
	if (!isAction)
	{
		state = PlayerState::Stay;
	}

	ItemAction();
	Attack();
	Blown();
	Move();

	switch (state)
	{
	case Player::Stay:
		if (old != state)
		{
			anim->PlayAnimation(0, 0.3f);
			smoke->Emit(false);
		}
		break;
	case Player::Walk:
		if (old != state)
		{
			anim->PlayAnimation(1, 0.2f);
			smoke->Emit(false);
		}
		break;
	case Player::Dash:
		if (old != state)
		{
			anim->PlayAnimation(2, 0.2f);
			smoke->Emit(true);
		}
		break;
	case Player::Punch:
		smoke->Emit(false);
		if(!anim->GetPlaying())
		{
			state = Player::Stay;
			anim->PlayAnimation(0, 0.1f);
			isAction = false;
		}
		break;
	case Player::Kick_Charge:
		if (anim->GetPlaying())
		{
			//チャージ開始
		}else
		{
			//キック
			state = PlayerState::Kick_Shot;
			CharagePower = 2.0f;
			anim->PlayAnimation(5, 0.2f, 1);
			anim->SetAnimeSpeed(1.5f);
		}
		break;
	case Player::Kick_Shot:
		if (!anim->GetPlaying())
		{
			state = Player::Stay;
			anim->PlayAnimation(0, 0.1f);
			isAction = false;
		}
		break;
	case Player::Damage:
		if (!anim->GetPlaying())
		{
			state = Player::Stay;
			anim->PlayAnimation(0, 0.1f);
			isAction = false;
		}
		break;
	case Player::Blow:
		/*if (!anim->GetPlaying())
		{
			state = Player::Stay;
			anim->PlayAnimation(0, 0.1f);
			isAction = false;
		}*/
		break;
	default:
		break;
	}
	old = state;
	//場外にでた。
	if (transform->localPosition.y < -200)
	{
		stock--;
		//ストックの数値更新
		Pparameter->SetStock(stock);
		damage = 0;
		Pparameter->SetDamage(damage);
		blown = Vector3::zero;
		rigor = 0;
		//残機があるなら
		if (stock > 0)
		{
			//スタートポジションに移動
			transform->localPosition = Vector3(0, 100, 0);
			transform->localAngle = Vector3::zero;
		}
		else
		{
			//死
			this->Active(false);
			idxPlate->Active(false);
			rankList->push_front(this);
		}
	}
}

void Player::LateUpdate()
{
	idxPlate->transform->localPosition = transform->LocalPos(Vector3(-50, 150, 0));
	idxPlate->transform->Update();
	idxPlate->transform->LockAt(GameObjectManager::mainCamera->gameObject);
}

void Player::Render()
{
	int test = 10;
}

void Player::Attack()
{
	//Bボタンでパンチ
	if ((KeyBoardInput->isPush(DIK_Z) || XboxInput(playeridx)->isPushButton(XINPUT_GAMEPAD_B)) &&
		!isAction)
	{
		state = PlayerState::Punch;
		//パンチ再生
		anim->PlayAnimation(3, 0.2f, 1);
		anim->SetAnimeSpeed(1.0f);
		isAction = true;
	}

	//Aボタンでキック
	if ((KeyBoardInput->isPressed(DIK_SPACE) || XboxInput(playeridx)->IsPressButton(XINPUT_GAMEPAD_A)) &&
		!isAction)
	{
		//蹴りため開始
		state = PlayerState::Kick_Charge;
		anim->PlayAnimation(4, 0.2f, 1);
		isAction = true;
	}
	//ボタンが離されたなら
	if(state == PlayerState::Kick_Charge &&
		(!KeyBoardInput->isPressed(DIK_SPACE) && !XboxInput(playeridx)->IsPressButton(XINPUT_GAMEPAD_A)))
	{
		//キックする
		state = PlayerState::Kick_Shot;
		CharagePower = anim->GetNowTime();
		anim->PlayAnimation(5, 0.2f, 1);
		anim->SetAnimeSpeed(1.0f);
	}

	double nowTime = anim->GetNowTime();

	//パンチの攻撃判定
	if(state == PlayerState::Punch &&
		0.27f < nowTime && nowTime < 0.3f)
	{
		//PunchSound->Play(false);
		//あたり判定を出す
		AttackCollision* obj = GameObjectManager::AddNew<AttackCollision>("panch", 0);
		Transform* t = obj->GetComponent<Transform>();
		//前に出す
		t->localPosition = transform->Local(Vector3(0, 60, 30));
		t->localAngle = transform->localAngle;
		t->UpdateTransform();

		DamageCollision::DamageCollisonInfo info;
		//攻撃力補正
		info.damage = Random::Range(1, 3) * Cparameter.power;
		info.blown = transform->Direction(Vector3(0.0f, 0.5f, 0.8f));
		//吹き飛ばし補正
		info.blown.Scale(Cparameter.blowCorrection);
		info.rigor = 0.2f;
		obj->Initialize(playeridx, 0.2f, Vector3(40, 30, 40), info);
	}

	//キックの攻撃判定
	if (state == PlayerState::Kick_Shot &&
		0.35f < nowTime && nowTime < 0.38f)
	{
		//あたり判定を出す
		AttackCollision* obj = GameObjectManager::AddNew<AttackCollision>("kick", 0);
		Transform* t = obj->GetComponent<Transform>();
		//前に出す
		t->localPosition = transform->Local(Vector3(0, 50, 35));
		t->localAngle = transform->localAngle;
		t->UpdateTransform();

		DamageCollision::DamageCollisonInfo info;
		//攻撃力補正
		info.damage = (Random::Range(5, 8) * (CharagePower+0.3f)) * Cparameter.power;
  		info.blown = transform->Direction(Vector3(0.0f, 1.5f, 3.0f));
		//吹き飛ばし補正
		info.blown.Scale(Cparameter.blowCorrection * CharagePower);
		info.rigor = 0.6f;
		obj->Initialize(playeridx, 0.2f, Vector3(60, 30, 70), info);
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
			if (KeyBoardInput->isPush(DIK_Z) || XboxInput(playeridx)->isPushButton(XINPUT_GAMEPAD_B) &&
				!isAction)
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

void Player::Blown()
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
			//攻撃判定へのベクトル
			Vector3 vec = co->transform->position - transform->position;
			vec.y = 0;
			vec.Normalize();
			//前
			Vector3 front = transform->Direction(Vector3::front);
			front.y = 0;
			front.Normalize();

			//transform->localAngle.y = D3DXToDegree(atan2f(vec.z, vec.x) - atan2f(front.z, front.x));
			//ベクトルから角度を求める
			//float rot = D3DXToRadian(360) - atan2f(vec.z, vec.x);
			//回転
			//transform->localAngle.y = D3DXToDegree(-rot + D3DXToRadian(0));

			//ダメージ加算
			if (damage + co->info.damage < 1000)
			{
				damage += co->info.damage;
			}
			else
			{
				damage = 999;
			}
			rigor = co->info.rigor;
			blown = co->info.blown;
			//吹き飛び補正
			blown.Scale(Cparameter.toBlowCorrection);
			//ダメージによる吹き飛び補正
			blown.Scale(1.0f + (float)damage / 50.0f);
			//コントローラ振動
			XboxInput(playeridx)->Vibration(20000, 20000);
			time = 0;
			smoke->Emit(true);

			Pparameter->SetDamage(damage);
			if (co->gameObject->Name() == "panch")
			{
				//ダメージひるみモーション
				anim->PlayAnimation(6, 0.2f, 1);
				state = Player::PlayerState::Damage;
				isAction = true;
			}else if (co->gameObject->Name() == "kick")
			{
				//吹き飛び
				anim->PlayAnimation(7, 0.2f,1);
				state = Player::PlayerState::Blow;
				isAction = true;
			}
		}
	}

	//ぶっ飛び計算
	time += Time::DeltaTime();
	//硬直時間以下
	if (time <= rigor)
	{
		transform->localPosition.Add(blown);
	}
	else if(rigor != 0)
	{
		//前の時間から1秒になるまでの時間
		float s = (float)Time::DeltaTime() - (time - rigor);
		blown.Scale(s);
		transform->localPosition.Add(blown);
		blown = Vector3::zero;
		rigor = 0;
		//コントローラ振動
		XboxInput(playeridx)->Vibration(0, 0);
		if (state != PlayerState::Dash)
			smoke->Emit(false);

		if (state == Player::PlayerState::Blow)
		{
			anim->PlayAnimation(0, 0.2f);
			state = Player::PlayerState::Stay;
			isAction = false;
		}
	}
}

void Player::Move()
{
	if (!isAction)
	{
		float s = Cparameter.speed;
		bool run = false;
		//ダッシュ
		if (KeyBoardInput->isPressed(DIK_LSHIFT) || XboxInput(playeridx)->IsPressButton(XINPUT_GAMEPAD_LEFT_SHOULDER))
		{
			run = true;
			s *= 1.5f;
		}
		Camera* c = GameObjectManager::mainCamera;
		Vector3 dir = Vector3::zero;
		//コントローラー移動
		dir.x = (XboxInput(playeridx)->GetAnalog(AnalogInput::L_STICK).x / 32767.0f)*s;
		dir.z = (XboxInput(playeridx)->GetAnalog(AnalogInput::L_STICK).y / 32767.0f)*s;
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

		//移動したか
		if (dir.Length() != 0)
		{
			if (run)
				state = PlayerState::Dash;
			else
				state = PlayerState::Walk;

			//カメラからみた向きに変換
			dir = c->transform->Direction(dir);
			dir.y = 0.0f;
			transform->localPosition.Add(Vector3(dir.x, dir.y, dir.z));

			//今の向きを求める

			//正規化
			dir.Normalize();
			//ベクトルから角度を求める
			float rot = D3DXToRadian(360) - atan2f(dir.z, dir.x);
			//回転
			transform->localAngle.y = D3DXToDegree(rot + D3DXToRadian(90));
		}
		////ジャンプ
		//if (KeyBoardInput->isPush(DIK_SPACE) || XboxInput(playeridx)->isPushButton(XINPUT_GAMEPAD_A))
		//{
		//	if (jump == false)
		//	{
		//		jump = true;
		//	}
		//}

		//if (jump)
		//{
		//	if(rigid->IsGround())
		//	{
		//		jump = false;
		//	}
		//	//上に移動
		//	transform->localPosition.Add(Vector3(0, 4, 0));
		//}
	}
}