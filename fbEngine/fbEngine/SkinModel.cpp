#include "SkinModel.h"
#include "Effect.h"
#include "Camera.h"
#include "Light.h"
#include "ShadowCamera.h"


//extern UINT                 g_NumBoneMatricesMax;
//extern D3DXMATRIXA16*       g_pBoneMatrices ;
bool g_PreRender = false;

SkinModel::SkinModel(GameObject * g, Transform * t) :
	Component(g, t, typeid(this).name()),
	_Effect(nullptr),
	_ModelDate(nullptr),
	_Camera(nullptr),
	_Light(nullptr),
	_ShadowCamera(nullptr),
	_TextureBlend(Color::white),
	_AllBlend(Color::white),
	_ModelEffect(ModelEffectE(ModelEffectE::CAST_SHADOW | ModelEffectE::RECEIVE_SHADOW)),
	_SkyBox(false)
{
	
}

SkinModel::~SkinModel()
{
	SAFE_DELETE(_ModelDate)
}

//再帰関数
void SkinModel::DrawFrame(LPD3DXFRAME pFrame)
{
	D3DXMESHCONTAINER_DERIVED* pMeshContainer;

	pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pFrame->pMeshContainer;
	//繋がっているメッシュコンテナがなくなるまで続ける
	while (pMeshContainer != NULL)
	{
		//メッシュ描画
		DrawMeshContainer(
			pMeshContainer,
			pFrame
			);
		//次のメッシュコンテナ
		//一つのフレームにコンテナは複数ある？
		pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pMeshContainer->pNextMeshContainer;
	}

	//兄弟が居るならを兄弟を読み込む
	if (pFrame->pFrameSibling != NULL)
	{
		DrawFrame(pFrame->pFrameSibling);
	}

	//子が居るなら子を読み込む
	if (pFrame->pFrameFirstChild != NULL)
	{
		DrawFrame(pFrame->pFrameFirstChild);
	}
}

void SkinModel::Awake()
{
	//mainのものが設定されているならセットされる。
	_Camera = GameObjectManager::mainCamera;
	_Light = GameObjectManager::mainLight;
	_ShadowCamera = GameObjectManager::mainShadowCamera;
}

//モデルデータの行列更新
void SkinModel::LateUpdate()
{
	//モデルデータがあるなら
	if (_ModelDate)
	{
		//掛けるワールド行列
		D3DXMATRIX wolrd;
		D3DXMatrixIdentity(&wolrd);
		wolrd = transform->GetWorldMatrix();
		
		_ModelDate->UpdateBoneMatrix(wolrd);	//行列を更新。
	}
}

void SkinModel::PreRender()
{
	g_PreRender = true;
	//モデルデータがあるなら
	if (_ModelDate)
	{
		//再帰関数呼び出し
		DrawFrame(_ModelDate->GetFrameRoot());
	}
}

void SkinModel::Render()
{
	g_PreRender = false;
	//モデルデータがあるなら
	if (_ModelDate)
	{
		//再帰関数呼び出し
		DrawFrame(_ModelDate->GetFrameRoot());
	}
}

void SkinModel::DrawMeshContainer(
	D3DXMESHCONTAINER_DERIVED* pMeshContainer,
	LPD3DXFRAME pFrameBase
)
{
	D3DXFRAME_DERIVED* pFrame = (D3DXFRAME_DERIVED*)pFrameBase;
	
	//事前描画
	if(g_PreRender)
	{
		//影描画
		if (_ModelEffect & ModelEffectE::CAST_SHADOW)
			CreateShadow(pMeshContainer, pFrame);
	}
	//モデル描画
	else
	{
		//エフェクト読み込み
		if (pMeshContainer->pSkinInfo != NULL)
			_Effect = EffectManager::LoadEffect("AnimationModel.fx");
		else
			_Effect = EffectManager::LoadEffect("3Dmodel.fx");

		//テクニックをセット
		_Effect->SetTechnique("NormalRender");
		//開始（必ず終了すること）
		_Effect->Begin(NULL, D3DXFX_DONOTSAVESHADERSTATE);
		_Effect->BeginPass(0);

		const int num = GameObjectManager::mainLight->GetNum();
		Vector4 dir[System::MAX_LIGHTNUM];
		Color color[System::MAX_LIGHTNUM];
		ZeroMemory(dir, sizeof(Vector4)*System::MAX_LIGHTNUM);
		const vector<DirectionalLight*>& vec = GameObjectManager::mainLight->GetLight();
		FOR(i, num)
		{
			dir[i] = vec[i]->Direction();
			color[i] = vec[i]->GetColor();
		}
		
		//ライトの向きを転送。
		_Effect->SetValue("g_diffuseLightDirection", &dir, sizeof(Vector4)*System::MAX_LIGHTNUM);
		//ライトのカラーを転送。
		_Effect->SetValue("g_diffuseLightColor", &color, sizeof(Color)*System::MAX_LIGHTNUM);
		//ライト数セット
		_Effect->SetInt("g_LightNum", num);
		//環境光
		_Effect->SetVector("g_ambientLight", &D3DXVECTOR4(0.4, 0.4, 0.4, 1.0f));

		//カメラのポジションセット(スペキュラライト用)
		Vector3 campos = GameObjectManager::mainCamera->transform->GetPosition();
		_Effect->SetValue("g_cameraPos", &D3DXVECTOR4(campos.x, campos.y, campos.z, 1.0f), sizeof(D3DXVECTOR4));
		
		//各行列を送信
		_Effect->SetMatrix("g_rotationMatrix", transform->GetRotateMatrixAddress());
		_Effect->SetMatrix("g_viewMatrix", &(D3DXMATRIX)_Camera->GetViewMat());
		_Effect->SetMatrix("g_projectionMatrix", &(D3DXMATRIX)_Camera->GetProjectionMat());
		
		//影カメラのビュープロジェクション行列を作って送信
		D3DXMATRIX LVP = GameObjectManager::mainShadowCamera->GetViewMat() * GameObjectManager::mainShadowCamera->GetProjectionMat();
		_Effect->SetMatrix("g_LVP", &LVP);

		//深度テクスチャ
		TEXTURE* shadow = INSTANCE(RenderTargetManager)->GetRTTextureFromList(RTIdxE::SHADOWDEPTH);
		_Effect->SetTexture("g_Shadow", shadow->pTexture);
		Vector2 texel = Vector2(1.0f / shadow->Size.x, 1.0f / shadow->Size.y);
		_Effect->SetValue("g_TexelSize", &texel,sizeof(Vector2));
		//影を映すかどうかのフラグセット
		_Effect->SetBool("ReceiveShadow", (_ModelEffect & ModelEffectE::RECEIVE_SHADOW) > 0);
		//スペキュラフラグセット
		_Effect->SetBool("Spec", (_ModelEffect & ModelEffectE::SPECULAR) > 0);

		_Effect->SetVector("g_blendcolor", (D3DXVECTOR4*)&_AllBlend);

		if(_SkyBox)
		{
			(*graphicsDevice()).SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		}

		(*graphicsDevice()).SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		(*graphicsDevice()).SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		(*graphicsDevice()).SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		//アニメーションの有無で分岐
		if (pMeshContainer->pSkinInfo != NULL)
		{
			UINT iAttrib;
			//バッファー
			LPD3DXBONECOMBINATION pBoneComb = LPD3DXBONECOMBINATION(pMeshContainer->pBoneCombinationBuf->GetBufferPointer());
			//各マテリアル
			for (iAttrib = 0; iAttrib < pMeshContainer->NumAttributeGroups; iAttrib++)
			{
				//ボーン
				for (DWORD iPaletteEntry = 0; iPaletteEntry < pMeshContainer->NumPaletteEntries; ++iPaletteEntry)
				{
					DWORD iMatrixIndex = pBoneComb[iAttrib].BoneId[iPaletteEntry];
					if (iMatrixIndex != UINT_MAX)
					{
						//骨の最終的な行列計算
						D3DXMatrixMultiply(
							&_BoneMatrixPallets[iPaletteEntry],
							//骨のオフセット(移動)行列
							&pMeshContainer->pBoneOffsetMatrices[iMatrixIndex],
							//フレームのワールド行列
							pMeshContainer->ppBoneMatrixPtrs[iMatrixIndex]
						);
					}
				}

				//骨のワールド行列配列
				_Effect->SetMatrixArray("g_mWorldMatrixArray", _BoneMatrixPallets, pMeshContainer->NumPaletteEntries);
				//骨の数
				_Effect->SetFloat("g_numBone", (float)pMeshContainer->NumInfl);

				//ディフューズカラー取得
				D3DXVECTOR4* Diffuse = (D3DXVECTOR4*)&pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D.Diffuse;
				//テクスチャー
				LPDIRECT3DTEXTURE9 texture = pMeshContainer->ppTextures[pBoneComb[iAttrib].AttribId];
				//テクスチャが格納されていればセット
				if (texture != NULL)
				{
					// ディフューズテクスチャ。
					_Effect->SetTexture("g_diffuseTexture", texture);
					_Effect->SetVector("g_Textureblendcolor", (D3DXVECTOR4*)&Color::white);
					//とりあえず、今回はテクスチャの名前を見る
					
					if (strcmp(pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].pTextureFilename, "TV_Head.png") == 0)
						_Effect->SetVector("g_Textureblendcolor", (D3DXVECTOR4*)&_TextureBlend);
					_Effect->SetBool("Texflg", true);
				}
				//テクスチャがないならカラーセット
				else if (Diffuse != NULL)
				{
					_Effect->SetVector("g_diffuseMaterial", Diffuse);
					_Effect->SetBool("Texflg", false);
				}

				//この関数を呼び出すことで、データの転送が確定する。
				_Effect->CommitChanges();
				//メッシュ描画
				pMeshContainer->MeshData.pMesh->DrawSubset(iAttrib);
			}
		}
		//アニメーションしない方
		else
		{
			_Effect->SetMatrix("g_worldMatrix", &pFrame->CombinedTransformationMatrix);

			//マテリアルの数
			DWORD MaterialNum = pMeshContainer->NumMaterials;
			//マテリアル
			D3DXMATERIAL *mtrl = (D3DXMATERIAL*)(pMeshContainer->pMaterials);

			//モデル描画
			for (DWORD i = 0; i < MaterialNum; i++)
			{
				//ディフューズカラー
				D3DXVECTOR4* Diffuse = (D3DXVECTOR4*)&mtrl[i].MatD3D.Diffuse;
				//テクスチャー
				LPDIRECT3DBASETEXTURE9 texture;
				_Effect->SetBool("SkyBox", _SkyBox);
				if(_SkyBox)
				{
					texture = pMeshContainer->ppCubeTextures[i];
				}
				else
				{
					texture = pMeshContainer->ppTextures[i];
				}

				//テクスチャが格納されていればセット
				if (texture != NULL)
				{
					_Effect->SetTexture("g_Texture", texture);
					_Effect->SetVector("g_Textureblendcolor", (D3DXVECTOR4*)&Color::white);
					_Effect->SetBool("Texflg", true);
				}
				//テクスチャがないならカラーセット
				else
				{
					_Effect->SetVector("g_diffuseMaterial", Diffuse);
					_Effect->SetBool("Texflg", false);
				}

				//この関数を呼び出すことで、データの転送が確定する。
				_Effect->CommitChanges();
				//メッシュ描画
				pMeshContainer->MeshData.pMesh->DrawSubset(i);
			}
		}

		_Effect->EndPass();
		_Effect->End();

		if (_SkyBox)
		{
			(*graphicsDevice()).SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
		}

		(*graphicsDevice()).SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		(*graphicsDevice()).SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
		(*graphicsDevice()).SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
	}
}

void SkinModel::CreateShadow(D3DXMESHCONTAINER_DERIVED * pMeshContainer, D3DXFRAME_DERIVED * pFrame)
{
	//エフェクト読み込み
	if (pMeshContainer->pSkinInfo != NULL)
		_Effect = EffectManager::LoadEffect("AnimationModel.fx");
	else
		_Effect = EffectManager::LoadEffect("3Dmodel.fx");
	//テクニック設定
	_Effect->SetTechnique("Shadow");

	//開始
	_Effect->Begin(0, D3DXFX_DONOTSAVESTATE);
	_Effect->BeginPass(0);

	//影カメラのビュープロジェクション行列を送る
	_Effect->SetMatrix("g_viewMatrix", &(D3DXMATRIX)GameObjectManager::mainShadowCamera->GetViewMat());
	_Effect->SetMatrix("g_projectionMatrix", &(D3DXMATRIX)GameObjectManager::mainShadowCamera->GetProjectionMat());

	//アニメーションの有無で分岐
	if (pMeshContainer->pSkinInfo != NULL)
	{
		UINT iAttrib;
		//バッファー
		LPD3DXBONECOMBINATION pBoneComb = LPD3DXBONECOMBINATION(pMeshContainer->pBoneCombinationBuf->GetBufferPointer());
		//各マテリアル
		for (iAttrib = 0; iAttrib < pMeshContainer->NumMaterials; iAttrib++)
		{
			//ボーン
			for (DWORD iPaletteEntry = 0; iPaletteEntry < pMeshContainer->NumPaletteEntries; ++iPaletteEntry)
			{
				DWORD iMatrixIndex = pBoneComb[iAttrib].BoneId[iPaletteEntry];
				if (iMatrixIndex != UINT_MAX)
				{
					//骨の最終的な行列計算
					D3DXMatrixMultiply(
						&_BoneMatrixPallets[iPaletteEntry],
						//骨のオフセット(移動)行列
						&pMeshContainer->pBoneOffsetMatrices[iMatrixIndex],
						//フレームのワールド行列
						pMeshContainer->ppBoneMatrixPtrs[iMatrixIndex]
					);
				}
			}

			//骨のワールド行列配列
			_Effect->SetMatrixArray("g_mWorldMatrixArray", _BoneMatrixPallets, pMeshContainer->NumPaletteEntries);
			//骨の数
			_Effect->SetFloat("g_numBone", (float)pMeshContainer->NumInfl);
	

			//この関数を呼び出すことで、データの転送が確定する。
			_Effect->CommitChanges();
			//メッシュ描画
			pMeshContainer->MeshData.pMesh->DrawSubset(iAttrib);
		}
	}
	else
	{
		_Effect->SetMatrix("g_worldMatrix", &pFrame->CombinedTransformationMatrix);

		//モデル描画
		for (DWORD i = 0; i < pMeshContainer->NumMaterials; i++)
		{
			//この関数を呼び出すことで、データの転送が確定する。
			_Effect->CommitChanges();
			//メッシュ描画
			pMeshContainer->MeshData.pMesh->DrawSubset(i);
		}
	}

	//終了
	_Effect->EndPass();
	_Effect->End();
}