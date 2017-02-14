#include "PlayerParameter.h"
#include "fbEngine/Sprite.h"
#include "fbEngine/TextObject.h"
#include "fbEngine/ImageObject.h"

void PlayerParameter::Awake()
{
	emblem = GameObjectManager::AddNew<ImageObject>("Emblem", 0);
	damageT = GameObjectManager::AddNew<TextObject>("Damage", 0);
	stockT = GameObjectManager::AddNew<TextObject>("Stock", 0);
	nameT = GameObjectManager::AddNew<TextObject>("Name", 0);
	//e‚É“o˜^
	emblem->transform->SetParent(transform);
	damageT->transform->SetParent(transform);
	stockT->transform->SetParent(transform);
	nameT->transform->SetParent(transform);
	//Á‚³‚ê‚È‚¢‚æ‚¤‚É
	emblem->Discard(false);
	damageT->Discard(false);
	stockT->Discard(false);
	nameT->Discard(false);

	emblem->SetTexture(TextureManager::LoadTexture("test.png"));

	stockT->SetFontStyle("HGS–¾’©E");
	stockT->SetString(L"3");
	stockT->SetFontSize(40.0f);

	damageT->SetFontStyle("HGS–¾’©E");
	damageT->SetString(L"0%");
	damageT->SetBlendColor(Color::white);
	damageT->SetFontSize(80.0f);
	damageT->transform->localPosition = Vector3(40, 20, 0);

	nameT->SetFontStyle("HGS–¾’©E");
	nameT->SetString(L"name");
	nameT->SetFontSize(30.0f);
	nameT->transform->localPosition = Vector3(0, 70, 0);
}

void PlayerParameter::Update()
{
}

void PlayerParameter::SetDamage(int damage)
{
	//ƒ_ƒ[ƒW
	wchar_t s[10];
	InttoString(damage, s);
	
	wcscat_s(s, wcslen(s) + wcslen(L"%") + 1, L"%");
	damageT->SetString(s);
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
	wchar_t s[10];
	InttoString(stock, s);
	stockT->SetString(s);
}

void PlayerParameter::SetName(wchar_t * name)
{
	nameT->SetString(name);
}
