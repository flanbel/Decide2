#include "PlayerParameter.h"
#include "fbEngine/Sprite.h"
#include "fbEngine/TextObject.h"
#include "fbEngine/ImageObject.h"

void PlayerParameter::Awake()
{
	emblem = GameObjectManager::AddNew<ImageObject>("Emblem", 0);
	frame = GameObjectManager::AddNew<ImageObject>("Frame", 0);
	killT = GameObjectManager::AddNew<TextObject>("Kill", 0);
	damageT = GameObjectManager::AddNew<TextObject>("Damage", 0);
	stockT = GameObjectManager::AddNew<TextObject>("Stock", 0);
	nameT = GameObjectManager::AddNew<TextObject>("Name", 0);
	//親に登録
	emblem->transform->SetParent(transform);
	frame->transform->SetParent(transform);
	damageT->transform->SetParent(transform);
	stockT->transform->SetParent(transform);
	killT->transform->SetParent(transform);
	nameT->transform->SetParent(transform);
	//消されないように
	emblem->Discard(false);
	frame->Discard(false);
	damageT->Discard(false);
	stockT->Discard(false);
	killT->Discard(false);
	nameT->Discard(false);

	emblem->SetTexture(TextureManager::LoadTexture("nico2.png"));
	emblem->SetShadow(true);
	emblem->transform->localPosition = Vector3(90, 0, 0);
	//frame->SetTexture(TextureManager::LoadTexture("Frame.png"));

	stockT->Initialize(L"3", 40.0f, Color::white, true, "HGS明朝E");
	stockT->transform->localPosition = Vector3(10, 0, 0);

	killT->Initialize(L"KILL:0", 40.0f, Color::white, true, "HGS明朝E");
	killT->transform->localPosition = Vector3(40, 0, 0);

	damageT->Initialize(L"0%", 80.0f, Color::white, true, "HGS明朝E");
	damageT->transform->localPosition = Vector3(0, 40, 0);

	nameT->Initialize(L"name", 30.0f, Color::white, true, "HGS明朝E");
	nameT->transform->localPosition = Vector3(0, 100, 0);
}

void PlayerParameter::Update()
{
}

void PlayerParameter::SetDamage(int damage)
{
	//ダメージ
	wchar_t display[10];
	InttoString(damage, display);
	//ダメージによって位置を変えるよ～(ほんとはテキストで設定したいよ)
	if(damage < 10)
	{
		damageT->transform->localPosition = Vector3(50, 40, 0);
	}else if(damage < 100)
	{
		damageT->transform->localPosition = Vector3(30, 40, 0);
	}else
	{
		damageT->transform->localPosition = Vector3(0, 40, 0);
	}
	wcscat_s(display, wcslen(display) + wcslen(L"%") + 1, L"%");
	damageT->SetString(display);
	if(damage < 300)
	{
		damageT->SetBlendColor(Color(1.0f, 1.0f - ((float)damage / 300.0f), 1.0f - ((float)damage / 200.0f), 1.0f));
	}else if(damage > 300)
	{
		damageT->SetBlendColor(Color(1.0f - ((float)damage / 999.0f) +0.4f, 0.0f, 0.0f, 1.0f));
	}
}

void PlayerParameter::SetStock(int stock)
{
	wchar_t display[10];
	if (stock >= 0)
	{
		InttoString(stock, display);
		stockT->SetString(display);
	}
	else
	{
		stockT->SetString(L"∞");
	}
}

void PlayerParameter::SetKill(int kill)
{
	wchar_t display[20],K[5];
	//コピー
	wcscpy_s(display, wcslen(L"KILL:") + 1, L"KILL:");
	InttoString(kill, K);
	//付け足し
	wcscat_s(display, 20, K);
	killT->SetString(display);
}

void PlayerParameter::SetName(wchar_t * name)
{
	nameT->SetString(name);
}

void PlayerParameter::SetColor(Color C)
{
	color = C;
	emblem->SetBlendColor(Color(color.r, color.g, color.b, 1.0f));
}
