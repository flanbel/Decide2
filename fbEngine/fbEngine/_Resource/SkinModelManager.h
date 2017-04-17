#pragma once

class SkinModelManager
{
public:	
	//3Dモデルを読み込み,モデルのデータ値を返す
	//第一引数：string ファイルの名前
	static SkinModelData* LoadModel(const char* filename);
private:
	static map<UINT64, SkinModelData*> modelDataMap;
};