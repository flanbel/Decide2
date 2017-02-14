#pragma once

#define WindowW 1280.0f
#define WindowH 720.0f
//プレイヤーの数
#define PLAYER_NUM 4
//選択可能なキャラクターの数
#define CHARACTER_NUM 1

enum COLLISION_ID
{
	ATTACK_1P = 1,
	ATTACK_2P,
	ATTACK_3P,
	ATTACK_4P,
	ITEM
};