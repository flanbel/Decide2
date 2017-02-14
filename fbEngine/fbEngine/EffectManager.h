#pragma once
class Effect;

class EffectManager
{
public:
	~EffectManager();
	static Effect* LoadEffect(char* filename);

private:
	static map<char*, Effect*> effectDictinary;
};