#pragma once
#include "Player.h"
#include "AttackCollision.h"
#include "fbEngine\_Object\_GameObject\AnimationPlate.h"
#include "fbEngine\_Object\_GameObject\SoundSource.h"

//アイテム種類の最大数
static const int ITEM_MAX_NUM = 2;

namespace 
{
	//各アイテムの投げられてヒットした時の動作を設定
	static void Standard(Item* item, vector<Collision*> colls)
	{
		//あたったやつに小ダメージを与える。
		FOR(i, colls.size())
		{
			//属性を確認
			if ((colls[i]->GetCollisonObj()->getUserIndex() & Collision_Attribute::PLAYER) != 0)
			{
				Player* player = (Player*)colls[i]->gameObject;
				Vector3 blow = item->GetMove();
				blow.Normalize();
				blow.Scale(30.0f);
				player->Damage(item->GetOwner(), Random::Range(6, 14), blow, 0.5f);
			}
		}
	};

	static void Bom(Item* item, vector<Collision*> colls)
	{
		//爆発アニメーション生成
		AnimationPlate *bomber = INSTANCE(GameObjectManager)->AddNew<AnimationPlate>("Bomber", 4);
		bomber->transform->SetPosition(item->transform->GetPosition());
		bomber->SetTexture(LOADTEXTURE("bomber.jpg"));
		bomber->SetSplit(6, 4, 6 * 4);
		bomber->SetSize(Vector2(256, 256));
		bomber->SetBlendMode(fbEngine::BlendModeE::Add);
		bomber->Play(1.0f, 1);
		bomber->SetBlendColor(Color::white * 1.8f);
		bomber->SetDelete(true);

		//爆発の効果音再生
		SoundSource* bomberSE = INSTANCE(GameObjectManager)->AddNew<SoundSource>("PunchSound", 0);
		bomberSE->Init("Asset/Sound/bomb3.wav");
		bomberSE->SetDelete(true);
		bomberSE->Play(false);

		//爆発の当たり判定を出す。
		AttackCollision* damage = INSTANCE(GameObjectManager)->AddNew<AttackCollision>("damage", 0);
		damage->transform->SetPosition(item->transform->GetPosition());
		SphereCollider* sphere = damage->AddComponent<SphereCollider>();
		sphere->Create(60.0f);
		DamageCollision::DamageCollisonInfo info;
		info.Damage = Random::Range(20, 45);
		info.Blown = item->GetMove() + Vector3(0, 20, 0);
		info.Blown.Normalize();
		info.Blown.Scale(180);
		info.Rigor = 1;
		float life = 0.2f;
		damage->Create(sphere, item->GetOwner(), Collision_Attribute::DAMAGE, life, info);

		//爆発したのでオブジェクトはなくなる
		INSTANCE(GameObjectManager)->AddRemoveList(item);
	};

	//関数ポインタの配列
	static void(*ItemActionArray[ITEM_MAX_NUM])(Item* item, vector<Collision*> colls) =
	{
		&Standard,
		&Bom
	};
}

namespace ItemInfo
{
	enum ItemTypeE
	{
		Throw,	//投げるタイプ
		Slash,	//振るタイプ
	};

	//アイテムのデータ
	struct ItemData
	{
		char* modelname;		//モデルのパス
		Vector3 collisionsize;	//当たり判定の大きさ
		Vector3 offset;			//当たり判定の移動量
		int itemtype;			//アイテムのタイプ
		void(*itemAction)(Item* item, vector<Collision*> colls);	//投げた時に使う関数のアドレス
	};

	//アイテムの情報
	static const ItemData itemList[ITEM_MAX_NUM] =
	{
		{
			"Item/StandardSowrd.X",
			Vector3(15,80,15),
			Vector3(0,40,0),
			ItemTypeE::Slash,
			&Standard
		},
		{
			"Item/bom.X",
			Vector3(30,35,30),
			Vector3(0,0,0),
			ItemTypeE::Throw,
			&Bom
		}
	};
}