#pragma once
//マテリアルクラス
class Material:Noncopyable
{
public:
	Material();
	~Material();
	//マテリアルの複製
	Material* CloneMaterial();
private:
	TEXTURE *_Diffuse;		//拡散反射光
	Color _BlendDiffuse;	//拡散反射光に混ぜる色
	TEXTURE *_Specular;		//鏡面反射光
	TEXTURE *_Normal;		//法線
	TEXTURE *_Luminance;	//輝度
	TEXTURE *_Occlusion;	//閉所の陰
	TEXTURE *_Height;		//高さ
	Color _Emission;		//自己発光
};