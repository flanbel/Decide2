#include "SkinModelManager.h"
#include "SkinModelData.h"

map<UINT64, SkinModelData*> SkinModelManager::modelDataMap;

SkinModelData* SkinModelManager::LoadModel(const char* filename)
{
	int hash = Support::MakeHash(filename);
	//“o˜^‚³‚ê‚Ä‚È‚¢
	if (modelDataMap[hash] == nullptr)
	{
		//¶¬
		SkinModelData* Original = new SkinModelData();
		//ƒtƒ@ƒCƒ‹ƒpƒX¶¬
		char* filepath = new char[64];
		strcpy_s(filepath,strlen("Asset/Xfile/")+1,"Asset/Xfile/");
		strcat_s(filepath, strlen(filepath) + strlen(filename)+1, filename);
		//“Ç‚ÝŽæ‚è
		Original->LoadModelData(filepath);
		//“o˜^
		modelDataMap[hash] = Original;
	}
	//’l‚ðˆø‚Á’£‚Á‚Ä‚­‚é
	return modelDataMap[hash];
}