#include "RenderTargetManager.h"

RenderTargetManager*  RenderTargetManager::_Instance;

RenderTarget::RenderTarget()
{
	texture = new TEXTURE();
}

void RenderTarget::Create(Vector2 size, _D3DFORMAT colorfmt)
{
	//前のテクスチャがあるなら
	if(texture->pTexture)
	{
		//削除
		SAFE_DELETE(texture->pTexture);
	}
	//テクスチャ作成
	HRESULT hr = (*graphicsDevice()).CreateTexture(
		size.x, size.y,
		1,
		D3DUSAGE_RENDERTARGET,
		colorfmt,
		D3DPOOL_DEFAULT,
		&texture->pTexture,
		NULL);

	//画像サイズを画面サイズに
	texture->Size = size;

	//ビューポート設定
	viewport = { 0, 0, (DWORD)size.x, (DWORD)size.y, 0.0f, 1.0f };

	//サーフェスのインターフェース取得
	texture->pTexture->GetSurfaceLevel(0, &buffer);

	//深度バッファ作成
	hr = (*graphicsDevice()).CreateDepthStencilSurface(
		size.x, size.y,
		D3DFMT_D24S8,
		D3DMULTISAMPLE_NONE,
		0,
		TRUE,
		&depth,
		NULL);
}

RenderTargetManager::RenderTargetManager()
{
	if (_BackBuffer == nullptr)
	{
		_BackBuffer = new RenderTarget();

		//バックバッファ(レンダリングターゲット)保持
		//参照カウンタがあがる
		(*graphicsDevice()).GetRenderTarget(0, &_BackBuffer->buffer);
		//深度バッファ保持
		(*graphicsDevice()).GetDepthStencilSurface(&_BackBuffer->depth);
		//ビューポート保持
		(*graphicsDevice()).GetViewport(&_BackBuffer->viewport);
		//参照カウンタを下げる
		//_BackBuffer->buffer->Release();

		//レンダリングターゲット数取得
		D3DCAPS9 Caps;
		(*graphicsDevice()).GetDeviceCaps(&Caps);
		DWORD MaxRT = Caps.NumSimultaneousRTs;
		_MaxRTNum = MaxRT;

		FOR(i,_MaxRTNum)
		{
			RenderTarget* ren = new RenderTarget();
			//とりあえず浮動小数点バッファで作ってみる(絶対無駄がある)
			ren->Create(g_WindowSize*2);
			ren->texture->Size = g_WindowSize;
			SetRT(i, ren);
			_RTList.push_back(ren);
		}
	}
}

void RenderTargetManager::ReSetRT(DWORD Index, RenderTarget* renderTarget, Color color)
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

void RenderTargetManager::ReSetRT()
{
	FOR(i,_MaxRTNum)
	{
		ReSetRT(i, _RTList[i]);
	}
}

void RenderTargetManager::SetRT(DWORD Index, RenderTarget* renderTarget)
{
	//テクスチャをレンダリングターゲットに
	(*graphicsDevice()).SetRenderTarget(Index, renderTarget->buffer);
	//テクスチャ用の深度バッファ作成
	(*graphicsDevice()).SetDepthStencilSurface(renderTarget->depth);
	//ビューポートを変更に
	(*graphicsDevice()).SetViewport(&renderTarget->viewport);
}

void RenderTargetManager::SetRT(DWORD Index, int idx)
{
	//テクスチャをレンダリングターゲットに
	(*graphicsDevice()).SetRenderTarget(Index, _RTList[idx]->buffer);
	//テクスチャ用の深度バッファ作成
	(*graphicsDevice()).SetDepthStencilSurface(_RTList[idx]->depth);
	//ビューポートを変更に
	(*graphicsDevice()).SetViewport(&_RTList[idx]->viewport);
}

void RenderTargetManager::RemoveRT(DWORD idx)
{
	(*graphicsDevice()).SetRenderTarget(idx, NULL);
	(*graphicsDevice()).SetDepthStencilSurface(NULL);
}

void RenderTargetManager::BeforeRT()
{
	//0～最大数のレンダーターゲットを退避
	for (short i = 0; i < _MaxRTNum; i++)
	{
		RemoveRT(i);
	}
	//元に戻す
	SetRT(0, _BackBuffer);
}

TEXTURE* RenderTargetManager::GetRTTextureFromList(DWORD idx)
{
	return _RTList[idx]->texture;
}

RenderTarget* RenderTargetManager::GetRTFromList(DWORD idx)
{
	return _RTList[idx];
}