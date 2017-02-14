/*!
 *@brief	スキンモデルデータ
 */
#pragma once
class Animation;

struct D3DXFRAME_DERIVED : public D3DXFRAME {
	D3DXMATRIXA16	CombinedTransformationMatrix;	//合成済み行列。
};
//メッシュコンテナを継承した拡張メッシュコンテナ
struct D3DXMESHCONTAINER_DERIVED : public D3DXMESHCONTAINER {
	//テクスチャ
	LPDIRECT3DTEXTURE9* ppTextures;
	//キューブ
	LPDIRECT3DCUBETEXTURE9* ppCubeTextures;
	//
	LPD3DXMESH pOrigMesh;
	//
	LPD3DXATTRIBUTERANGE pAttributeTable;
	//
	DWORD NumAttributeGroups;
	//
	DWORD NumInfl;
	//
	LPD3DXBUFFER pBoneCombinationBuf;
	//フレームの合成済み行列への参照
	D3DXMATRIX** ppBoneMatrixPtrs;
	//ボーンのオフセット行列
	D3DXMATRIX* pBoneOffsetMatrices;
	//
	DWORD NumPaletteEntries;
	//
	bool UseSoftwareVP;
	//
	DWORD iAttributeSW;
};

//モデルのデータを扱うクラス
class SkinModelData:Noncopyable
{
public:
	/*!
	 *@brief	コンストラクタ。
	 */
	SkinModelData();
	/*!
	 *@brief	デストラクタ。
	 */
	~SkinModelData();
	/*!
	 * @brief	モデルデータをロード。
	 *@param[in]	filePath	ファイルパス。
	 */
	void LoadModelData(const char* filePath);
	//モデルデータのクローン作製
	void CloneModelData(const SkinModelData* modelData, Animation* anim = nullptr);
	

	/*!
	* @brief	リリース。
	*/
	void Release();

	//最初(根元？)のフレーム取得
	LPD3DXFRAME GetFrameRoot()
	{
		return m_frameRoot;
	}

	ID3DXAnimationController* GetAnimationController()
	{
		return m_pAnimationController;
	}

	//引数として受け取った行列を親として,骨の行列を更新
	//第一引数：D3DXMATRIX&　ワールド行列
	void UpdateBoneMatrix(const D3DXMATRIX& matWorld);
	LPD3DXMESH GetOrgMeshFirst() const;
private:
	LPD3DXFRAME					m_frameRoot;		//フレームルート。
	//アニメーションコントローラー
	ID3DXAnimationController* m_pAnimationController;

	LPD3DXMESH GetOrgMesh(LPD3DXFRAME frame) const;

	void CloneSkeleton(LPD3DXFRAME& dstFrame, LPD3DXFRAME srcFrame);
	void DeleteCloneSkeleton(LPD3DXFRAME frame);
	void SetupOutputAnimationRegist(LPD3DXFRAME frame, ID3DXAnimationController* animCtr);
};