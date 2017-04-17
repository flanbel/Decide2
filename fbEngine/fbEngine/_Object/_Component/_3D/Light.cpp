#include "Light.h"

Light::~Light()
{
	for each (auto lightp in _LightVec)
	{
		//É|ÉCÉìÉ^çÌèú
		SAFE_DELETE(lightp);
	}
	_LightVec.clear();
}

void Light::Awake()
{
	_LightVec.clear();
}

void Light::AddLight(DirectionalLight* l)
{
	_LightVec.push_back(l);
}

const vector<DirectionalLight*>& Light::GetLight()
{
	return _LightVec;
}

const int Light::GetNum() const
{
	return min(System::MAX_LIGHTNUM,(const int)_LightVec.size());
}