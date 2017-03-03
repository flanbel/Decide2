#include "Bloom.h"
#include "Effect.h"
#include "RenderTargetManager.h"
#include "ImageObject.h"
#include "Sprite.h"

void Bloom::Create()
{
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
			//未実装postEffect->RenderFullScreen();

			Effect->EndPass();
			Effect->End();

		}//輝度抽出

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

					//未実装postEffect->RenderFullScreen();

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

					//未実装postEffect->RenderFullScreen();

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

			//未実装postEffect->RenderFullScreen();

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

			//未実装postEffect->RenderFullScreen();

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
}
