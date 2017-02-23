#pragma once
#include "Component.h"
#include "SkinModelData.h"

class Effect;
class Camera;
class Light;
class ShadowCamera;

//影を落とすか
enum CastShadow
{
	Off,	//落とさない
	On,		//落とす
	//ShadowOnly
};

//モデルの描画を行うクラス
class SkinModel:public Component{
public:
	SkinModel::SkinModel(GameObject* g, Transform* t) :Component(g, t)
	{
		name = (char*)typeid(*this).name();
	}
	~SkinModel();

	void Awake()override;
	void LateUpdate()override;
	void PreRender()override;
	void Render()override;

	void SetCamera(Camera* c) 
	{
		camera = c;
	}
	void SetLight(Light* l)
	{
		light = l;
	}

	//モデルデータへアクセスするためのポインタ設定
	void SetModelData(SkinModelData* pD)
	{
		modelDate = pD;
	}
	//メッシュコライダー作るときに
	LPD3DXMESH GetOrgMeshFirst() const
	{
		return modelDate->GetOrgMeshFirst();
	}
	void SetReceive(bool f)
	{
		receiveShadow = f;
	}
	void SetSpecular(bool f)
	{
		Specular = f;
	}
	void SetSky(bool f)
	{
		SkyBox = f;
	}
	void SetTextureBlend(Color c)
	{
		TextureBlend = c;
	}

	void SetAllBlend(Color c)
	{
		AllBlend = c;
	}
private:
	//エフェクトへの参照
	Effect* effect;
	//モデルデータへアクセスするためのポインタ保持
	SkinModelData* modelDate;
	Camera* camera;
	Light* light = nullptr;
	ShadowCamera* shadowCamera;
	//ブレンドする色
	Color TextureBlend, AllBlend;

	//影を生成するときの設定
	CastShadow castShadow;
	//影を受けるかどうかの設定
	bool receiveShadow;
	//スペキュラライトフラグ
	bool Specular;
	//スカイボックスかどうか(正直どうかと思うが・・・。)
	bool SkyBox = false;

	//プライベート関数

	//子とか兄弟も一括で描画するための再帰関数
	void DrawFrame(LPD3DXFRAME pFrame);
	
	//モデル描画
	void SkinModel::DrawMeshContainer(
		D3DXMESHCONTAINER_DERIVED* pMeshContainerBase,
		LPD3DXFRAME pFrameBase
		);
	
	//影作成
	void CreateShadow(D3DXMESHCONTAINER_DERIVED* pMeshContainer, D3DXFRAME_DERIVED* pFrame);
};