#pragma once
#include "fbEngine/GameObject.h"

class TextObject;
class ImageObject;

//パーセントを表示したいなぁ
class PlayerParameter:public GameObject
{
public:
	PlayerParameter(char* name) :GameObject(name) {}
	void Awake()override;
	void Update()override;
	void SetDamage(int damage);
	void SetStock(int stock);
	void SetName(wchar_t* name);
private:
	//エンブレム
	ImageObject* emblem;
	TextObject *damageT,*stockT,*nameT;
};