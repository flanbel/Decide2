#include "Light.h"

void Light::Awake()
{
	color = Color::white;
	lightVec.clear();
}

void Light::AddLight(DirectionalLight l)
{
	lightVec.push_back(l);
}

const vector<DirectionalLight>& Light::GetLight()
{
	return lightVec;
}

int Light::GetNum()
{
	return MAX_LIGHTNUM < (int)lightVec.size() ? MAX_LIGHTNUM : (int)lightVec.size();
}
