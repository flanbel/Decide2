#pragma once
#include "fbEngine/GameObject.h"

class TextObject;
class ImageObject;

//パーセントを表示したいなぁ
class PlayerParameter:public GameObject
{
public:
	PlayerParameter(const char* name) :GameObject(name) {}
	void Awake()override;
	void Update()override;
	void SetDamage(int damage);
	void SetStock(int stock);
	void SetKill(int kill);
	void SetName(const wchar_t* name);
	void SetColor(Color C);
private:
	//エンブレム
	ImageObject* _Emblem,*_Frame;
	TextObject *_DamageT,*_StockT,*_KillT,*_NameT;
	Color _Color;
};