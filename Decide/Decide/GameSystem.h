#pragma once

static const float WindowW = 1280.0f;
static const float WindowH = 720.0f;
//プレイヤーの数
static const int PLAYER_NUM = 4;
//選択可能なキャラクターの数
static const int CHARACTER_NUM = 1;

namespace
{
	enum Collision_Attribute
	{
		PLAYER = fbCollisionAttributeE::CHARACTER,
		STAGE = fbCollisionAttributeE::GROUND,
		ITEM = BIT(3),
		DAMAGE = BIT(5),
	};

	enum class Collision_Filter : short
	{
		PLAYER = (short)fbCollisionFilterE::A,
		STAGE = (short)fbCollisionFilterE::B,
		ITEM = (short)fbCollisionFilterE::C,
		DAMAGE = (short)fbCollisionFilterE::D,
	};
}