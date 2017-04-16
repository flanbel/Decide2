#pragma once

static const float WindowW = 1280.0f;
static const float WindowH = 720.0f;
//プレイヤーの数
static const int PLAYER_NUM = 4;
//選択可能なキャラクターの数
static const int CHARACTER_NUM = 1;

namespace
{
	enum Collision_ID
	{
		PLAYER = fbCollisionAttributeE::CHARACTER,
		STAGE = fbCollisionAttributeE::GROUND,
		ITEM = BIT(3),
		DAMAGE = BIT(5),
		ATTACK_1P,
		ATTACK_2P,
		ATTACK_3P,
		ATTACK_4P,
	};

	enum class Collision_Filter : short
	{
		PLAYER = (Collision_Filter)fbCollisionFilterE::A,
		STAGE = (Collision_Filter)fbCollisionFilterE::B,
		ITEM = (Collision_Filter)fbCollisionFilterE::C,
		DAMAGE = (Collision_Filter)fbCollisionFilterE::D,
	};
}