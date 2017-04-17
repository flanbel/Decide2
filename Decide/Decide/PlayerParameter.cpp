#include "PlayerParameter.h"

void PlayerParameter::Awake()
{
	_Emblem = INSTANCE(GameObjectManager)->AddNew<ImageObject>("Emblem", 0);
	_Frame = INSTANCE(GameObjectManager)->AddNew<ImageObject>("Frame", 0);
	_KillT = INSTANCE(GameObjectManager)->AddNew<TextObject>("Kill", 0);
	_DamageT = INSTANCE(GameObjectManager)->AddNew<TextObject>("Damage", 0);
	_StockT = INSTANCE(GameObjectManager)->AddNew<TextObject>("Stock", 0);
	_NameT = INSTANCE(GameObjectManager)->AddNew<TextObject>("Name", 0);
	//親に登録
	_Emblem->transform->SetParent(transform);
	_Frame->transform->SetParent(transform);
	_DamageT->transform->SetParent(transform);
	_StockT->transform->SetParent(transform);
	_KillT->transform->SetParent(transform);
	_NameT->transform->SetParent(transform);
	//消されないように
	_Emblem->SetDiscard(false);
	_Frame->SetDiscard(false);
	_DamageT->SetDiscard(false);
	_StockT->SetDiscard(false);
	_KillT->SetDiscard(false);
	_NameT->SetDiscard(false);

	_DamageT->Initialize(L"0%", 100.0f, Color::white, fbSprite::SpriteEffectE::SHADOW, "HGS明朝E",fbText::TextFormatE::RIGHT);

	_NameT->Initialize(L"name", 40.0f, Color::white, fbSprite::SpriteEffectE::SHADOW, "HGS明朝E");
	_NameT->transform->SetLocalPosition(Vector3(-40, 40, 0));

	_StockT->Initialize(L"3", 40.0f, Color::white, fbSprite::SpriteEffectE::SHADOW, "HGS明朝E");
	_StockT->transform->SetLocalPosition(Vector3(-110, -80, 0));

	_KillT->Initialize(L"KILL:0", 40.0f, Color::white, fbSprite::SpriteEffectE::SHADOW, "HGS明朝E");
	_KillT->transform->SetLocalPosition(Vector3(60, -80, 0));

	_Emblem->SetTexture(LOADTEXTURE("nico2.png"));
	_Emblem->SetEffectFlg(fbSprite::SpriteEffectE::SHADOW, true);
	_Emblem->transform->SetLocalPosition(Vector3(50, -20, 0));
	//_Frame->SetTexture(LOADTEXTURE("Frame.png"));
}

void PlayerParameter::Update()
{
}

void PlayerParameter::SetDamage(int damage)
{
	//ダメージ
	wchar_t display[10];
	Support::ToString(damage, display);
	wcscat_s(display, wcslen(display) + wcslen(L"%") + 1, L"%");
	_DamageT->SetString(display);
	//ダメージの色変更
	if(damage < 300)
	{
		//だんだん赤く
		_DamageT->SetBlendColor(Color(1.0f, 1.0f - ((float)damage / 300.0f), 1.0f - ((float)damage / 200.0f), 1.0f));
	}else if(damage > 300)
	{
		//だんだん黒く
		_DamageT->SetBlendColor(Color(1.0f - ((float)damage / 999.0f) +0.4f, 0.0f, 0.0f, 1.0f));
	}
}

void PlayerParameter::SetStock(int stock)
{
	wchar_t display[10];
	if (stock >= 0)
	{
		Support::ToString(stock, display);
		_StockT->SetString(display);
	}
	else
	{
		_StockT->SetString(L"∞");
	}
}

void PlayerParameter::SetKill(int kill)
{
	wchar_t display[20],K[5];
	//コピー
	wcscpy_s(display, wcslen(L"KILL:") + 1, L"KILL:");
	Support::ToString(kill, K);
	//付け足し
	wcscat_s(display, 20, K);
	_KillT->SetString(display);
}

void PlayerParameter::SetName(const wchar_t * name)
{
	_NameT->SetString(name);
}

void PlayerParameter::SetColor(Color C)
{
	_Color = C;
	_Emblem->SetBlendColor(Color(_Color.r, _Color.g, _Color.b, 1.0f));
}
