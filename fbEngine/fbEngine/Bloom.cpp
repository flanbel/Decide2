#include "Bloom.h"
#include "Effect.h"
#include "RenderTargetManager.h"
#include "Vertex.h"
#include "VertexDefinition.h"

void Bloom::Create()
{
	if (_Vertex == nullptr)
	{
		_Vertex = new Vertex();

		//ポジション定義
		VERTEX_POSITION position[] = {
			{ -1.0f, -1.0f, 0.0f, 1.0f },//左下
			{ -1.0f, 1.0f, 0.0f, 1.0f },//左上
			{ 1.0f, -1.0f, 0.0f, 1.0f },//右下
			{ 1.0f, 1.0f, 0.0f, 1.0f },//右上
		};
		//UV定義
		VERTEX_TEXCOORD texcoord[] = {
			{ 0.0f, 1.0f },//左下
			{ 0.0f, 0.0f },//左上
			{ 1.0f, 1.0f },//右下
			{ 1.0f, 0.0f },//右上
		};

		//頂点宣言(頂点がどのように構成されているか)
		D3DVERTEXELEMENT9 elements[] = {
			{ 0, 0              , D3DDECLTYPE_FLOAT4  , D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 }, // 頂点座標
			{ 1, 0              , D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD   , 0 }, // UV
			D3DDECL_END()
		};

		_Vertex->Initialize(PRIMITIVETYPE::TRIANGLESTRIP, 4);
		_Vertex->CreateVertexBuffer(position, 4, sizeof(VERTEX_POSITION), elements[0]);
		_Vertex->CreateVertexBuffer(texcoord, 4, sizeof(VERTEX_TEXCOORD), elements[1]);
		_Vertex->CreateDeclaration();
	}

	//有効フラグをコピー
	isEnable_ = true;

	if (isEnable_)
	{
		//内部解像度のサイズを計算
		int w = g_WindowSize.x;
		int h = g_WindowSize.y;

		//輝度レンダリングターゲットの作成
		LuminanceRT = new RenderTarget();
		LuminanceRT->Create(g_WindowSize);

		for (int i = 0; i < NUM_DOWN_SAMPLING_RT / 2; i++)
		{
			//シフト量の計算
			int shift = i + 1;

			//ダウンサンプリング用RTの添え字を計算
			int baseIndex = i * 2;

			//シフトしてサイズを小さくしていく(縮小バッファの作成)

			//横ブラー用RTの作成
			DownSamplingRT[baseIndex] = new RenderTarget();
			DownSamplingRT[baseIndex]->Create(Vector2(w >> shift, h >> (shift - 1)));
			//縦ブラー用RTの作成
			DownSamplingRT[baseIndex + 1] = new RenderTarget();
			DownSamplingRT[baseIndex + 1]->Create(Vector2(w >> shift, h >> shift));
		}

		//ぼかし合成用RTの作成
		CombineRT = new RenderTarget();
		CombineRT->Create(Vector2(w >> 2, h >> 2));

		//エフェクトのロード
		Effect = EffectManager::LoadEffect("Bloom.fx");
	}
}

void Bloom::Render()
{
	if (isEnable_)
	{
		//重み計算
		UpdateWeight(25.0f);

		//輝度抽出
		{

			//輝度抽出用のレンダリングターゲットに変更
			(*graphicsDevice()).SetRenderTarget(0, LuminanceRT->buffer);
			(*graphicsDevice()).SetDepthStencilSurface(LuminanceRT->depth);

			//テクスチャのクリア
			(*graphicsDevice()).Clear(0, nullptr, D3DCLEAR_TARGET, 0, 1.0f, 0);

			Effect->SetTechnique("SamplingLuminance");

			Effect->Begin(NULL, D3DXFX_DONOTSAVESHADERSTATE);
			Effect->BeginPass(0);

			//テクスチャの設定
			//オフスクリーンしたやつ
			Effect->SetTexture("g_Scene", INSTANCE(SceneManager)->GetOffScreenTexture()->pTexture);

			Effect->CommitChanges();

			//画像描画
			_Vertex->DrawPrimitive();

			Effect->EndPass();
			Effect->End();

		}//輝度抽出
		D3DXVECTOR3 l, r;
		l = D3DXVECTOR3(2.0f, 2.0f, 2.0f);
		r = D3DXVECTOR3(0.2125f, 0.7154f, 0.0721f);
		float a = D3DXVec3Dot(&l, &r);

		 //輝度をぼかす
		{
			//ループ用RTクラスのポインタ
			RenderTarget* prevRT = LuminanceRT;
			//ダウンサンプリング用RTの添え字
			int rtIndex = 0;

			for (int i = 0; i < NUM_DOWN_SAMPLING_RT / 2; i++)
			{
				//XBlur
				{
					//ダウンサンプリング用RTのXBlur用をレンダリングターゲットに設定
					(*graphicsDevice()).SetRenderTarget(0, DownSamplingRT[rtIndex]->buffer);
					(*graphicsDevice()).SetDepthStencilSurface(DownSamplingRT[rtIndex]->depth);

					//画像をクリア
					(*graphicsDevice()).Clear(0, nullptr, D3DCLEAR_TARGET, 0, 1.0f, 0);

					//テクニックを設定
					Effect->SetTechnique("XBlur");

					Effect->Begin(NULL, D3DXFX_DONOTSAVESHADERSTATE);
					Effect->BeginPass(0);

					//画像サイズを計算
					float size[2] =
					{
						static_cast<float>(prevRT->texture->size.x),
						static_cast<float>(prevRT->texture->size.y)
					};
					//オフセットを計算
					float offset[] =
					{
						16.0f / static_cast<float>(prevRT->texture->size.x),
						0.0f
					};

					//画面サイズを設定
					Effect->SetValue("g_LuminanceTexSize", size, sizeof(size));
					//オフセットを設定
					Effect->SetValue("g_Offset", offset, sizeof(size));
					//重みを設定
					Effect->SetValue("g_Weight", Weights_, sizeof(Weights_));

					//テクスチャを設定
					Effect->SetTexture("g_Blur", prevRT->texture->pTexture);

					Effect->CommitChanges();

					//描画
					_Vertex->DrawPrimitive();

					Effect->EndPass();
					Effect->End();
				}//XBlur

				 //YBlur用を設定
				prevRT = DownSamplingRT[rtIndex];
				rtIndex++;

				//YBlur
				{
					//ダウンサンプリング用RTのYBlur用をレンダリングターゲットに設定
					(*graphicsDevice()).SetRenderTarget(0, DownSamplingRT[rtIndex]->buffer);
					(*graphicsDevice()).SetDepthStencilSurface(DownSamplingRT[rtIndex]->depth);

					//画像をクリア
					(*graphicsDevice()).Clear(0, nullptr, D3DCLEAR_TARGET, 0, 1.0f, 0);

					Effect->SetTechnique("YBlur");

					Effect->Begin(NULL, D3DXFX_DONOTSAVESHADERSTATE);
					Effect->BeginPass(0);

					//サイズを計算
					float size[2] =
					{
						static_cast<float>(prevRT->texture->size.x),	//横幅
						static_cast<float>(prevRT->texture->size.y)	//高さ
					};
					//オフセットを計算
					float offset[] =
					{
						0.0f,										//横幅
						16.0f / static_cast<float>(prevRT->texture->size.y),	//高さ
					};

					//画面サイズを設定
					Effect->SetValue("g_LuminanceTexSize", size, sizeof(size));
					//オフセットを設定
					Effect->SetValue("g_Offset", offset, sizeof(size));
					//重みを設定
					Effect->SetValue("g_Weight", Weights_, sizeof(Weights_));

					//テクスチャの設定
					Effect->SetTexture("g_Blur", prevRT->texture->pTexture);

					Effect->CommitChanges();

					//描画
					_Vertex->DrawPrimitive();

					Effect->EndPass();
					Effect->End();
				}

				//XBlur用を設定
				prevRT = DownSamplingRT[rtIndex];
				rtIndex++;

			}//YBlur

		}//輝度をぼかす

		 //ボケフィルターの合成
		{

			//アルファブレンドをなしに設定
			(*graphicsDevice()).SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

			(*graphicsDevice()).SetRenderTarget(0, CombineRT->buffer);
			(*graphicsDevice()).SetDepthStencilSurface(CombineRT->depth);

			//画像をクリア
			(*graphicsDevice()).Clear(0, nullptr, D3DCLEAR_TARGET, 0, 1.0f, 0);

			float offset[] =
			{
				0.5f / static_cast<float>(CombineRT->texture->size.x),
				0.5f / static_cast<float>(CombineRT->texture->size.y),
			};

			Effect->SetTechnique("Combine");

			Effect->Begin(NULL, D3DXFX_DONOTSAVESHADERSTATE);
			Effect->BeginPass(0);

			//テクスチャの設定
			Effect->SetTexture("g_CombineTex00", DownSamplingRT[1]->texture->pTexture);
			Effect->SetTexture("g_CombineTex01", DownSamplingRT[3]->texture->pTexture);
			Effect->SetTexture("g_CombineTex02", DownSamplingRT[5]->texture->pTexture);
			Effect->SetTexture("g_CombineTex03", DownSamplingRT[7]->texture->pTexture);
			Effect->SetTexture("g_CombineTex04", DownSamplingRT[9]->texture->pTexture);

			Effect->SetValue("g_Offset", offset, sizeof(offset));

			Effect->CommitChanges();

			//描画
			_Vertex->DrawPrimitive();

			Effect->EndPass();
			Effect->End();

		}//ボケフィルターの合成

		 //ブルーム
		{

			float offset[] =
			{
				0.5f / static_cast<float>(CombineRT->texture->size.x),
				0.5f / static_cast<float>(CombineRT->texture->size.y)
			};

			//メインレンダーに変更
			INSTANCE(RenderTargetManager)->BeforeRenderTarget();
			

			//加算合成。
			(*graphicsDevice()).SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			(*graphicsDevice()).SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
			(*graphicsDevice()).SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

			Effect->SetTechnique("Final");

			Effect->Begin(NULL, D3DXFX_DONOTSAVESHADERSTATE);
			Effect->BeginPass(0);

			Effect->SetTexture("g_Blur", CombineRT->texture->pTexture);
			Effect->SetValue("g_Offset", offset, sizeof(offset));

			Effect->CommitChanges();

			//描画
			_Vertex->DrawPrimitive();

			Effect->EndPass();
			Effect->End();

			(*graphicsDevice()).SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
			(*graphicsDevice()).SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			(*graphicsDevice()).SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		}//ブルーム

	}
}

void Bloom::Release()
{
}

void Bloom::UpdateWeight(float dis)
{
	float total = 0;

	for (int i = 0; i < NUM_WEIGHTS; i++)
	{
		Weights_[i] = expf(-0.5f*(float)(i*i) / dis);

		total += 2.0f * Weights_[i];
	}

	//規格化
	for (int i = 0; i < NUM_WEIGHTS; i++)
	{
		Weights_[i] /= total;
	}
}
