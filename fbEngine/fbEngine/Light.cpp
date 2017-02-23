#include "Light.h"

Light::~Light()
{
	for each (auto lightp in lightVec)
	{
		//É|ÉCÉìÉ^çÌèú
		SAFE_DELETE(lightp);
	}
	lightVec.clear();
}

void Light::Awake()
{
	lightVec.clear();
}

void Light::AddLight(DirectionalLight* l)
{
	lightVec.push_back(l);
}

const vector<DirectionalLight*> Light::GetLight()
{
	return lightVec;
}

int Light::GetNum()
{
	return min(MAX_LIGHTNUM,(int)lightVec.size());
}