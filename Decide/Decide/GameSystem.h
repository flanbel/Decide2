#pragma once

static const float WindowW = 1280.0f;
static const float WindowH = 720.0f;
//プレイヤーの数
static const int PLAYER_NUM = 4;
//選択可能なキャラクターの数
static const int CHARACTER_NUM = 1;

enum Collision_ID
{
	ATTACK_1P,
	ATTACK_2P,
	ATTACK_3P,
	ATTACK_4P,
	ITEM,
	PLAYER,
	STAGE = 7,
};