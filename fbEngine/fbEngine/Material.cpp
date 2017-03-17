#include "stdafx.h"
#include "Material.h"

Material::Material():
	_Diffuse(nullptr),
	_BlendDiffuse(Color::white),
	_Specular(nullptr),
	_Normal(nullptr),
	_Height(nullptr),
	_Occlusion(nullptr),
	_Luminance(nullptr),
	_Emission(Color::black)
{
}

Material::~Material()
{

}

Material * Material::CloneMaterial()
{
	return nullptr;
}
