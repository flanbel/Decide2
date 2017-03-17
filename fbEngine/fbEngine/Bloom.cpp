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

		_Vertex->Initialize(fbEngine::PRIMITIVETYPE::TRIANGLESTRIP, 4);
		_Vertex->CreateVertexBuffer(position, 4, sizeof(VERTEX_POSITION), elements[0]);
		_Vertex->CreateVertexBuffer(texcoord, 4, sizeof(VERTEX_TEXCOORD), elements[1]);
		_Vertex->CreateDeclaration();
	}

	//有効フラグをコピー
	_IsEnable = true;

	if (_IsEnable)
	{
		//内部解像度のサイズを計算
		int w = g_WindowSize.x / 2;
		int h = g_WindowSize.y / 2;

		//輝度レンダリングターゲットの作成
		_LuminanceRT = new RenderTarget();
		_LuminanceRT->Create(g_WindowSize, D3DFMT_A16B16G16R16F);

		for (int i = 0; i < NUM_DOWN_SAMPLING_RT / 2; i++)
		{
			//シフト量の計算
			int shift = i + 1;

			//ダウンサンプリング用RTの添え字を計算
			int baseIndex = i * 2;

			//シフトしてサイズを小さくしていく(縮小バッファの作成)

			//横ブラー用RTの作成
			_DownSamplingRT[baseIndex] = new RenderTarget();
			_DownSamplingRT[baseIndex]->Create(Vector2(w >> shift, h >> (shift - 1)), D3DFMT_A16B16G16R16F);
			//縦ブラー用RTの作成
			_DownSamplingRT[baseIndex + 1] = new RenderTarget();
			_DownSamplingRT[baseIndex + 1]->Create(Vector2(w >> shift, h >> shift), D3DFMT_A16B16G16R16F);
		}

		//ぼかし合成用RTの作成
		_CombineRT = new RenderTarget();
		_CombineRT->Create(Vector2(w >> 2, h >> 2), D3DFMT_A16B16G16R16F);

		//エフェクトのロード
		_Effect = EffectManager::LoadEffect("Bloom.fx");
	}
}

void Bloom::Render()
{
	if (_IsEnable)
	{
		//重み計算
		_UpdateWeight(25.0f);

		//輝度抽出
		{

			//輝度抽出用のレンダリングターゲットに変更
			(*graphicsDevice()).SetRenderTarget(0, _LuminanceRT->buffer);
			(*graphicsDevice()).SetDepthStencilSurface(_LuminanceRT->depth);

			//テクスチャのクリア
			(*graphicsDevice()).Clear(0, nullptr, D3DCLEAR_TARGET, 0, 1.0f, 0);

			_Effect->SetTechnique("SamplingLuminance");

			_Effect->Begin(NULL, D3DXFX_DONOTSAVESHADERSTATE);
			_Effect->BeginPass(0);

			//テクスチャの設定
			//オフスクリーンしたやつ
			_Effect->SetTexture("g_Scene", INSTANCE(SceneManager)->GetOffScreenTexture()->pTexture);

			_Effect->CommitChanges();

			//画像描画
			_Vertex->DrawPrimitive();

			_Effect->EndPass();
			_Effect->End();

		}//輝度抽出
		D3DXVECTOR3 l, r;
		l = D3DXVECTOR3(2.0f, 2.0f, 2.0f);
		r = D3DXVECTOR3(0.2125f, 0.7154f, 0.0721f);
		float a = D3DXVec3Dot(&l, &r);

		 //輝度をぼかす
		{
			//ループ用RTクラスのポインタ
			RenderTarget* prevRT = _LuminanceRT;
			//ダウンサンプリング用RTの添え字
			int rtIndex = 0;

			for (int i = 0; i < NUM_DOWN_SAMPLING_RT / 2; i++)
			{
				//XBlur
				{
					//ダウンサンプリング用RTのXBlur用をレンダリングターゲットに設定
					(*graphicsDevice()).SetRenderTarget(0, _DownSamplingRT[rtIndex]->buffer);
					(*graphicsDevice()).SetDepthStencilSurface(_DownSamplingRT[rtIndex]->depth);

					//画像をクリア
					(*graphicsDevice()).Clear(0, nullptr, D3DCLEAR_TARGET, 0, 1.0f, 0);

					//テクニックを設定
					_Effect->SetTechnique("XBlur");

					_Effect->Begin(NULL, D3DXFX_DONOTSAVESHADERSTATE);
					_Effect->BeginPass(0);

					//画像サイズを計算
					float size[2] =
					{
						static_cast<float>(prevRT->texture->Size.x),
						static_cast<float>(prevRT->texture->Size.y)
					};
					//オフセットを計算
					float offset[] =
					{
						16.0f / static_cast<float>(prevRT->texture->Size.x),
						0.0f
					};

					//画面サイズを設定
					_Effect->SetValue("g_LuminanceTexSize", size, sizeof(size));
					//オフセットを設定
					_Effect->SetValue("g_Offset", offset, sizeof(size));
					//重みを設定
					_Effect->SetValue("g_Weight", _Weights, sizeof(_Weights));

					//テクスチャを設定
					_Effect->SetTexture("g_Blur", prevRT->texture->pTexture);

					_Effect->CommitChanges();

					//描画
					_Vertex->DrawPrimitive();

					_Effect->EndPass();
					_Effect->End();
				}//XBlur

				 //YBlur用を設定
				prevRT = _DownSamplingRT[rtIndex];
				rtIndex++;

				//YBlur
				{
					//ダウンサンプリング用RTのYBlur用をレンダリングターゲットに設定
					(*graphicsDevice()).SetRenderTarget(0, _DownSamplingRT[rtIndex]->buffer);
					(*graphicsDevice()).SetDepthStencilSurface(_DownSamplingRT[rtIndex]->depth);

					//画像をクリア
					(*graphicsDevice()).Clear(0, nullptr, D3DCLEAR_TARGET, 0, 1.0f, 0);

					_Effect->SetTechnique("YBlur");

					_Effect->Begin(NULL, D3DXFX_DONOTSAVESHADERSTATE);
					_Effect->BeginPass(0);

					//サイズを計算
					float size[2] =
					{
						static_cast<float>(prevRT->texture->Size.x),	//横幅
						static_cast<float>(prevRT->texture->Size.y)	//高さ
					};
					//オフセットを計算
					float offset[] =
					{
						0.0f,										//横幅
						16.0f / static_cast<float>(prevRT->texture->Size.y),	//高さ
					};

					//画面サイズを設定
					_Effect->SetValue("g_LuminanceTexSize", size, sizeof(size));
					//オフセットを設定
					_Effect->SetValue("g_Offset", offset, sizeof(size));
					//重みを設定
					_Effect->SetValue("g_Weight", _Weights, sizeof(_Weights));

					//テクスチャの設定
					_Effect->SetTexture("g_Blur", prevRT->texture->pTexture);

					_Effect->CommitChanges();

					//描画
					_Vertex->DrawPrimitive();

					_Effect->EndPass();
					_Effect->End();
				}

				//XBlur用を設定
				prevRT = _DownSamplingRT[rtIndex];
				rtIndex++;

			}//YBlur

		}//輝度をぼかす

		 //ボケフィルターの合成
		{

			//アルファブレンドをなしに設定
			(*graphicsDevice()).SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

			(*graphicsDevice()).SetRenderTarget(0, _CombineRT->buffer);
			(*graphicsDevice()).SetDepthStencilSurface(_CombineRT->depth);

			//画像をクリア
			(*graphicsDevice()).Clear(0, nullptr, D3DCLEAR_TARGET, 0, 1.0f, 0);

			float offset[] =
			{
				0.5f / static_cast<float>(_CombineRT->texture->Size.x),
				0.5f / static_cast<float>(_CombineRT->texture->Size.y),
			};

			_Effect->SetTechnique("Combine");

			_Effect->Begin(NULL, D3DXFX_DONOTSAVESHADERSTATE);
			_Effect->BeginPass(0);

			//テクスチャの設定
			_Effect->SetTexture("g_CombineTex00", _DownSamplingRT[1]->texture->pTexture);
			_Effect->SetTexture("g_CombineTex01", _DownSamplingRT[3]->texture->pTexture);
			_Effect->SetTexture("g_CombineTex02", _DownSamplingRT[5]->texture->pTexture);
			_Effect->SetTexture("g_CombineTex03", _DownSamplingRT[7]->texture->pTexture);
			_Effect->SetTexture("g_CombineTex04", _DownSamplingRT[9]->texture->pTexture);

			_Effect->SetValue("g_Offset", offset, sizeof(offset));

			_Effect->CommitChanges();

			//描画
			_Vertex->DrawPrimitive();

			_Effect->EndPass();
			_Effect->End();

		}//ボケフィルターの合成

		 //ブルーム
		{

			float offset[] =
			{
				0.5f / static_cast<float>(_CombineRT->texture->Size.x),
				0.5f / static_cast<float>(_CombineRT->texture->Size.y)
			};

			//メインレンダーに変更
			INSTANCE(RenderTargetManager)->BeforeRT();
			

			//加算合成。
			(*graphicsDevice()).SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			(*graphicsDevice()).SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
			(*graphicsDevice()).SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

			_Effect->SetTechnique("Final");

			_Effect->Begin(NULL, D3DXFX_DONOTSAVESHADERSTATE);
			_Effect->BeginPass(0);

			_Effect->SetTexture("g_Blur", _CombineRT->texture->pTexture);
			_Effect->SetValue("g_Offset", offset, sizeof(offset));

			_Effect->CommitChanges();

			//描画
			_Vertex->DrawPrimitive();

			_Effect->EndPass();
			_Effect->End();

			(*graphicsDevice()).SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
			(*graphicsDevice()).SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			(*graphicsDevice()).SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		}//ブルーム

	}
}

void Bloom::Release()
{
}

void Bloom::_UpdateWeight(const float& dis)
{
	float total = 0;

	for (int i = 0; i < NUM_WEIGHTS; i++)
	{
		_Weights[i] = expf(-0.5f*(float)(i*i) / dis);

		total += 2.0f * _Weights[i];
	}

	//規格化
	for (int i = 0; i < NUM_WEIGHTS; i++)
	{
		_Weights[i] /= total;
	}
}
