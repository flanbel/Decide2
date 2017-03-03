#include "RenderTargetManager.h"

RenderTargetManager*  RenderTargetManager::instance;

RenderTarget::RenderTarget()
{
	texture = new TEXTURE();
}

void RenderTarget::Create(Vector2 size)
{
	//テクスチャ作成
	HRESULT hr = (*graphicsDevice()).CreateTexture(
		size.x, size.y,
		1,
		D3DUSAGE_RENDERTARGET,
		D3DFMT_A8R8G8B8,
		D3DPOOL_DEFAULT,
		&texture->pTexture,
		NULL);

	//画像サイズを画面サイズに
	texture->size = size;

	//ビューポート設定
	viewport = { 0, 0, (DWORD)size.x, (DWORD)size.y, 0.0f, 1.0f };

	//サーフェスのインターフェース取得
	texture->pTexture->GetSurfaceLevel(0, &buffer);

	//深度バッファ作成
	(*graphicsDevice()).CreateDepthStencilSurface(
		size.x, size.y,
		D3DFMT_D16,
		D3DMULTISAMPLE_NONE,
		0,
		TRUE,
		&depth,
		NULL);
}

RenderTargetManager::RenderTargetManager()
{
	if (backBuffer == nullptr)
	{
		backBuffer = new RenderTarget();

		//バックバッファ(レンダリングターゲット)保持
		//参照カウンタがあがる
		(*graphicsDevice()).GetRenderTarget(0, &backBuffer->buffer);
		//深度バッファ保持
		(*graphicsDevice()).GetDepthStencilSurface(&backBuffer->depth);
		//ビューポート保持
		(*graphicsDevice()).GetViewport(&backBuffer->viewport);
		//参照カウンタを下げる
		//backBuffer->buffer->Release();

		//レンダリングターゲット数取得
		D3DCAPS9 Caps;
		(*graphicsDevice()).GetDeviceCaps(&Caps);
		DWORD MaxRT = Caps.NumSimultaneousRTs;
		RenderTargetNum = MaxRT;

		FOR(RenderTargetNum)
		{
			RenderTarget* ren = new RenderTarget();
			ren->Create(g_WindowSize*2);
			SetRenderTarget(i, ren);
			renderTargerList.push_back(ren);
		}
	}
}

void RenderTargetManager::ReSetRenderTarget(DWORD Index, RenderTarget* renderTarget, Color color)
{
	//テクスチャをレンダリングターゲットに
	(*graphicsDevice()).SetRenderTarget(Index, renderTarget->buffer);
	//テクスチャ用の深度バッファ設定
	(*graphicsDevice()).SetDepthStencilSurface(renderTarget->depth);
	//ビューポートを変更に
	(*graphicsDevice()).SetViewport(&renderTarget->viewport);

	//レンダリングターゲット初期化
	(*graphicsDevice()).Clear(
		Index,
		NULL,
		D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
		D3DCOLOR_RGBA((int)(color.r*255), (int)(color.g*255), (int)(color.b * 255), (int)(color.a* 255)),
		1.0f,
		0);
}

void RenderTargetManager::SetRenderTarget(DWORD Index, RenderTarget* renderTarget)
{
	//テクスチャをレンダリングターゲットに
	(*graphicsDevice()).SetRenderTarget(Index, renderTarget->buffer);
	//テクスチャ用の深度バッファ作成
	(*graphicsDevice()).SetDepthStencilSurface(renderTarget->depth);
	//ビューポートを変更に
	(*graphicsDevice()).SetViewport(&renderTarget->viewport);
}

void RenderTargetManager::SetRenderTarget(DWORD Index, int idx)
{
	//テクスチャをレンダリングターゲットに
	(*graphicsDevice()).SetRenderTarget(Index, renderTargerList[idx]->buffer);
	//テクスチャ用の深度バッファ作成
	(*graphicsDevice()).SetDepthStencilSurface(renderTargerList[idx]->depth);
	//ビューポートを変更に
	(*graphicsDevice()).SetViewport(&renderTargerList[idx]->viewport);
}

void RenderTargetManager::ReSetRenderTarget()
{
	FOR(RenderTargetNum)
	{
		ReSetRenderTarget(i, renderTargerList[i]);
	}
}

void RenderTargetManager::BeforeRenderTarget()
{
	//0～最大数のレンダーターゲットを退避
	for(short i = 0;i < RenderTargetNum;i++)
	{
		RemoveRenderTarget(i);
	}
	//元に戻す
	SetRenderTarget(0, backBuffer);
}

TEXTURE* RenderTargetManager::GetRenderTargetTexture(DWORD idx)
{
	return renderTargerList[idx]->texture;
}

RenderTarget* RenderTargetManager::GetRenderTarget(DWORD idx)
{
	return renderTargerList[idx];
}

//void RenderTargetManager::CopyTexture(DWORD Idx,TEXTURE* tex)
//{
//	TEXTURE* me = renderTargerList[Idx]->texture;
//	//テクスチャの値をコピー
//	//memcpy_s(tex->pTexture, sizeof(IDirect3DTexture9), me->pTexture, sizeof(IDirect3DTexture9));
//	tex->pTexture = me->pTexture;
//	tex->size = me->size;
//}

void RenderTargetManager::RemoveRenderTarget(DWORD idx)
{
	(*graphicsDevice()).SetRenderTarget(idx, NULL);
}