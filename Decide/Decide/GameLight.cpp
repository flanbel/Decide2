#include "GameLight.h"
#include "fbEngine/Light.h"

void GameLight::Awake()
{
	Light* light = AddComponent<Light>();
	GameObjectManager::mainLight = light;
	int num = 4;
	DirectionalLight Dl[4];
	Dl[0].rot = Vector3(45, 45, 180);	//‰œ
	Dl[1].rot = Vector3(0, 0, 180);	//‰œ
	Dl[2].rot = Vector3(0, -90, 0);	//¶
	Dl[3].rot = Vector3(90, 0, 0);	//‰º

	Dl[0].color = Color(0.7, 0.7, 0.7, 1.0);	//‰œ
	Dl[1].color = Color(0.3, 0.3, 0.3, 0.1);	//‰E
	Dl[2].color = Color(0.3, 0.3, 0.3, 0.1);//¶
	Dl[3].color = Color(0.3, 0.3, 0.3, 0.1);	//‰º
	FOR(4)
	light->AddLight(Dl[i]);
}

void GameLight::Update()
{

}
