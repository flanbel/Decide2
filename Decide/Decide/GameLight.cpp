#include "GameLight.h"
#include "fbEngine/Light.h"

void GameLight::Awake()
{
	Light* light = AddComponent<Light>();
	GameObjectManager::mainLight = light;
	int num = 4;
	DirectionalLight* Dl[4];
	FOR(num)
		Dl[i] = new DirectionalLight();
	Dl[0]->SetEulerAngles(Vector3(45, 45, 180));	//‰œ
	Dl[1]->SetEulerAngles(Vector3(0, 0, 180));	//‰œ
	Dl[2]->SetEulerAngles(Vector3(0, -90, 0));	//¶
	Dl[3]->SetEulerAngles(Vector3(90, 0, 0));	//‰º
	
	Dl[0]->SetColor(Color(0.7f, 0.7f, 0.7f, 1.0f));	//‰œ
	Dl[1]->SetColor(Color(0.3f, 0.3f, 0.3f, 0.1f));	//‰E
	Dl[2]->SetColor(Color(0.3f, 0.3f, 0.3f, 0.1f));	//¶
	Dl[3]->SetColor(Color(0.3f, 0.3f, 0.3f, 0.1f));	//‰º
	FOR(num)
		light->AddLight(Dl[i]);
}

void GameLight::Update()
{

}
