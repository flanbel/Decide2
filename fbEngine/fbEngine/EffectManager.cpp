#include "EffectManager.h"
#include "Effect.h"

map<char*, Effect*> EffectManager::effectDictinary;

EffectManager::~EffectManager()
{
	map<char*, Effect*>::iterator it = effectDictinary.begin();
	while (it != effectDictinary.end())
	{
		SAFE_RELEASE(it->second);
		it++;
	}
	effectDictinary.clear();
}

Effect* EffectManager::LoadEffect(char* filename)
{
	Effect* effect = nullptr;

	auto it = effectDictinary.find(filename);
	//辞書内にエフェクトが登録されてない
	if (it == effectDictinary.end()) 
	{
		char* filepath = new char[64];
		strcpy_s(filepath, strlen("Asset/Shader/")+1, "Asset/Shader/");
		strcat_s(filepath, strlen(filepath) + strlen(filename)+1, filename);
		ID3DXEffect* ieffect;	//Effectへのポインタ
		LPD3DXBUFFER  compileErrorBuffer = NULL;
		//Effectファイル読み込み
		HRESULT hr = D3DXCreateEffectFromFile(
			graphicsDevice(),
			filepath,
			NULL,
			NULL,
#ifdef _DEBUG
			D3DXSHADER_DEBUG,
#else
			D3DXSHADER_SKIPVALIDATION,
#endif
			NULL,
			&ieffect,
			&compileErrorBuffer
			);

		if (FAILED(hr)) {
			MessageBox(NULL, (LPCSTR)(compileErrorBuffer->GetBufferPointer()), "error", MB_OK);
		}

		effect = new Effect(filename);
		effect->SetEffect(ieffect);
		//辞書に登録
		effectDictinary[filename] = effect;
	}
	else
	{
		effect = it->second;
	}

	return effect;
}