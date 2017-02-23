#include "SkinModel.h"
#include "Effect.h"
#include "Camera.h"
#include "Light.h"
#include "ShadowCamera.h"


extern UINT                 g_NumBoneMatricesMax;
extern D3DXMATRIXA16*       g_pBoneMatrices ;
bool g_PreRender = false;

SkinModel::~SkinModel()
{
	SAFE_DELETE(modelDate)
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
	camera = GameObjectManager::mainCamera;
	light = GameObjectManager::mainLight;
	shadowCamera = GameObjectManager::mainShadowCamera;
	castShadow = CastShadow::On;
	receiveShadow = true;
	Specular = false;

	SkyBox = false;
	AllBlend = Color::white;
	TextureBlend = Color::white;
}

//モデルデータの行列更新
void SkinModel::LateUpdate()
{
	//モデルデータがあるなら
	if (modelDate)
	{
		//掛けるワールド行列
		D3DXMATRIX wolrd;
		D3DXMatrixIdentity(&wolrd);
		wolrd = transform->WorldMatrix();
		
		modelDate->UpdateBoneMatrix(wolrd);	//行列を更新。
	}
}

void SkinModel::PreRender()
{
	g_PreRender = true;
	//モデルデータがあるなら
	if (modelDate)
	{
		//再帰関数呼び出し
		DrawFrame(modelDate->GetFrameRoot());
	}

}

void SkinModel::Render()
{
	g_PreRender = false;
	//モデルデータがあるなら
	if (modelDate)
	{
		//再帰関数呼び出し
		DrawFrame(modelDate->GetFrameRoot());
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
		if (castShadow)
			CreateShadow(pMeshContainer, pFrame);
	}
	//モデル描画
	else
	{
		//エフェクト読み込み
		if (pMeshContainer->pSkinInfo != NULL)
			effect = EffectManager::LoadEffect("AnimationModel.fx");
		else
			effect = EffectManager::LoadEffect("3Dmodel.fx");

		//テクニックをセット
		effect->SetTechnique("NormalRender");
		//開始（必ず終了すること）
		effect->Begin(NULL, D3DXFX_DONOTSAVESHADERSTATE);
		effect->BeginPass(0);

		int num = GameObjectManager::mainLight->GetNum();
		Vector4 dir[MAX_LIGHTNUM];
		Color color[MAX_LIGHTNUM];
		ZeroMemory(dir, sizeof(Vector4)*MAX_LIGHTNUM);
		const vector<DirectionalLight*> vec = GameObjectManager::mainLight->GetLight();
		FOR(num)
		{
			dir[i] = vec[i]->Direction();
			color[i] = vec[i]->color;
		}
		
		//ライトの向きを転送。
		effect->SetValue("g_diffuseLightDirection", &dir, sizeof(Vector4)*MAX_LIGHTNUM);
		//ライトのカラーを転送。
		effect->SetValue("g_diffuseLightColor", &color, sizeof(Color)*MAX_LIGHTNUM);
		//ライト数セット
		effect->SetInt("g_LightNum", num);
		//環境光
		effect->SetVector("g_ambientLight", &D3DXVECTOR4(0.3, 0.3, 0.3, 1.0f));

		//カメラのポジションセット(スペキュラライト用)
		Vector3 campos = GameObjectManager::mainCamera->transform->position;
		effect->SetValue("g_cameraPos", &D3DXVECTOR4(campos.x, campos.y, campos.z, 1.0f), sizeof(D3DXVECTOR4));
		
		//各行列を送信
		effect->SetMatrix("g_rotationMatrix", &transform->RotateMatrix());
		effect->SetMatrix("g_viewMatrix", &camera->View());
		effect->SetMatrix("g_projectionMatrix", &camera->Projection());
		
		//影カメラのビュープロジェクション行列を作って送信
		D3DXMATRIX LVP = shadowCamera->View() * shadowCamera->Projection();
		effect->SetMatrix("g_LVP", &LVP);

		//深度テクスチャ
		if (receiveShadow)
		{
			effect->SetTexture("g_Shadow", INSTANCE(RenderTargetManager)->GetRenderTargetTexture(0)->pTexture);
			effect->SetBool("ReceiveShadow",true);
		}else
		{
			
			effect->SetBool("ReceiveShadow", false);
		}
		//スペキュラライト
		effect->SetBool("Spec", Specular);
		effect->SetBool("SkyBox", SkyBox);

		effect->SetVector("g_blendcolor", (D3DXVECTOR4*)&AllBlend);

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
							&g_pBoneMatrices[iPaletteEntry],
							//骨のオフセット(移動)行列
							&pMeshContainer->pBoneOffsetMatrices[iMatrixIndex],
							//フレームのワールド行列
							pMeshContainer->ppBoneMatrixPtrs[iMatrixIndex]
						);
					}
				}

				//骨のワールド行列配列
				effect->SetMatrixArray("g_mWorldMatrixArray", g_pBoneMatrices, pMeshContainer->NumPaletteEntries);
				//骨の数
				effect->SetFloat("g_numBone", (float)pMeshContainer->NumInfl);
				// ボーン数。
				effect->SetInt("CurNumBones", pMeshContainer->NumInfl - 1);

				//ディフューズカラー取得
				D3DXVECTOR4* Diffuse = (D3DXVECTOR4*)&pMeshContainer->pMaterials[iAttrib].MatD3D.Diffuse;
				//テクスチャー
				LPDIRECT3DTEXTURE9 texture = pMeshContainer->ppTextures[iAttrib];
				//テクスチャが格納されていればセット
				if (texture != NULL)
				{
					// ディフューズテクスチャ。
					effect->SetTexture("g_diffuseTexture", texture);
					effect->SetVector("g_Textureblendcolor", (D3DXVECTOR4*)&Color::white);
					//とりあえず、今回はテクスチャの名前を見る
					
					if (strcmp(pMeshContainer->pMaterials[iAttrib].pTextureFilename, "TV_Head.png") == 0)
						effect->SetVector("g_Textureblendcolor", (D3DXVECTOR4*)&TextureBlend);
					effect->SetBool("Texflg", true);
				}
				//テクスチャがないならカラーセット
				else if (Diffuse != NULL)
				{
					effect->SetVector("g_diffuseMaterial", Diffuse);
					effect->SetBool("Texflg", false);
				}

				//この関数を呼び出すことで、データの転送が確定する。
				effect->CommitChanges();
				//メッシュ描画
				pMeshContainer->MeshData.pMesh->DrawSubset(iAttrib);
			}
		}
		//アニメーションしない方
		else
		{
			effect->SetMatrix("g_worldMatrix", &pFrame->CombinedTransformationMatrix);

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
				if(SkyBox)
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
					effect->SetTexture("g_Texture", texture);
					effect->SetVector("g_Textureblendcolor", (D3DXVECTOR4*)&Color::white);
					effect->SetBool("Texflg", true);
				}
				//テクスチャがないならカラーセット
				else
				{
					effect->SetVector("g_diffuseMaterial", Diffuse);
					effect->SetBool("Texflg", false);
				}

				//この関数を呼び出すことで、データの転送が確定する。
				effect->CommitChanges();
				//メッシュ描画
				pMeshContainer->MeshData.pMesh->DrawSubset(i);
			}
		}

		effect->EndPass();
		effect->End();

		(*graphicsDevice()).SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		(*graphicsDevice()).SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
		(*graphicsDevice()).SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
	}
}

void SkinModel::CreateShadow(D3DXMESHCONTAINER_DERIVED * pMeshContainer, D3DXFRAME_DERIVED * pFrame)
{
	//エフェクト読み込み
	if (pMeshContainer->pSkinInfo != NULL)
		effect = EffectManager::LoadEffect("AnimationModel.fx");
	else
		effect = EffectManager::LoadEffect("3Dmodel.fx");
	//テクニック設定
	effect->SetTechnique("Shadow");
	{
		//開始
		effect->Begin(0, D3DXFX_DONOTSAVESTATE);
		effect->BeginPass(0);

		//影カメラのビュープロジェクション行列を送る
		effect->SetMatrix("g_viewMatrix", &shadowCamera->View());
		effect->SetMatrix("g_projectionMatrix", &shadowCamera->Projection());

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
							&g_pBoneMatrices[iPaletteEntry],
							//骨のオフセット(移動)行列
							&pMeshContainer->pBoneOffsetMatrices[iMatrixIndex],
							//フレームのワールド行列
							pMeshContainer->ppBoneMatrixPtrs[iMatrixIndex]
						);
					}
				}

				//骨のワールド行列配列
				effect->SetMatrixArray("g_mWorldMatrixArray", g_pBoneMatrices, pMeshContainer->NumPaletteEntries);
				//骨の数]
				effect->SetFloat("g_numBone", (float)pMeshContainer->NumInfl);
				// ボーン数。
				effect->SetInt("CurNumBones", pMeshContainer->NumInfl - 1);

				//この関数を呼び出すことで、データの転送が確定する。
				effect->CommitChanges();
				//メッシュ描画
				pMeshContainer->MeshData.pMesh->DrawSubset(iAttrib);
			}
		}
		else
		{
			effect->SetMatrix("g_worldMatrix", &pFrame->CombinedTransformationMatrix);

			//モデル描画
			for (DWORD i = 0; i < pMeshContainer->NumMaterials; i++)
			{
				//この関数を呼び出すことで、データの転送が確定する。
				effect->CommitChanges();
				//メッシュ描画
				pMeshContainer->MeshData.pMesh->DrawSubset(i);
			}
		}

		//終了
		effect->EndPass();
		effect->End();
	}
}