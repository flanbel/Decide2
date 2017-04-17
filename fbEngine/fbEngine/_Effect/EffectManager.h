#pragma once
#include "Effect.h"

class EffectManager
{
public:
	~EffectManager();
	static Effect* LoadEffect(char* filename);

private:
	static map<char*, Effect*> effectDictinary;
};