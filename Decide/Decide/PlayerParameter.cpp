#include "PlayerParameter.h"
#include "fbEngine/Sprite.h"
#include "fbEngine/TextObject.h"
#include "fbEngine/ImageObject.h"

void PlayerParameter::Awake()
{
	emblem = GameObjectManager::AddNew<ImageObject>("Emblem", 0);
	frame = GameObjectManager::AddNew<ImageObject>("Frame", 0);
	damageT = GameObjectManager::AddNew<TextObject>("Damage", 0);
	stockT = GameObjectManager::AddNew<TextObject>("Stock", 0);
	killT = GameObjectManager::AddNew<TextObject>("Kill", 0);
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
	emblem->transform->localPosition = Vector3(90, 0, 0);
	//frame->SetTexture(TextureManager::LoadTexture("Frame.png"));

	stockT->SetFontStyle("HGS明朝E");
	stockT->SetString(L"3");
	stockT->SetFontSize(40.0f);
	stockT->transform->localPosition = Vector3(10, 10, 0);

	killT->SetFontStyle("HGS明朝E");
	killT->SetString(L"KILL:0");
	killT->SetFontSize(40.0f);
	killT->transform->localPosition = Vector3(40, 10, 0);

	damageT->SetFontStyle("HGS明朝E");
	damageT->SetString(L"0%");
	damageT->SetBlendColor(Color::white);
	damageT->SetFontSize(80.0f);
	damageT->transform->localPosition = Vector3(0, 40, 0);

	nameT->SetFontStyle("HGS明朝E");
	nameT->SetString(L"name");
	nameT->SetFontSize(30.0f);
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
	emblem->SetBlendColor(Color(color.r, color.g, color.b, 0.7f));
}
