#include "SkinModelManager.h"
#include "SkinModelData.h"

map<UINT64, SkinModelData*> SkinModelManager::modelDataMap;

SkinModelData* SkinModelManager::LoadModel(char* filename)
{
	int hash = MakeHash(filename);
	//“o˜^‚³‚ê‚Ä‚È‚¢
	if (modelDataMap[hash] == nullptr)
	{
		//¶¬
		SkinModelData* Original = new SkinModelData();
		//ƒtƒ@ƒCƒ‹ƒpƒX¶¬
		char* filepath = new char[64];
		strcpy_s(filepath,Length("Asset/Xfile/"),"Asset/Xfile/");
		strcat_s(filepath, Length(filepath) + Length(filename), filename);
		//“Ç‚ÝŽæ‚è
		Original->LoadModelData(filepath);
		//“o˜^
		modelDataMap[hash] = Original;
	}
	//’l‚ðˆø‚Á’£‚Á‚Ä‚­‚é
	return modelDataMap[hash];
}