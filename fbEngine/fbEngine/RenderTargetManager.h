#pragma once

//レンダーターゲットの情報
struct RenderTarget:Noncopyable
{
	//カラーバッファ
	IDirect3DSurface9* buffer;
	//深度バッファ
	IDirect3DSurface9* depth;
	//ビューポート
	D3DVIEWPORT9 viewport;
	//
	TEXTURE* texture;
	RenderTarget();
	void Create(Vector2 size);
};

//レンダリングターゲット管理クラス
class RenderTargetManager
{
public:
	//レンダーターゲットを保持
	RenderTargetManager();

	//レンダーターゲットを変更し、画面をクリアする
	//第一引数：DWORD 何番目のレンダーターゲットに設定するか,第ニ引数：RenderTarget*　設定するレンダーターゲット
	void ReSetRenderTarget(DWORD RenderTargetIndex, RenderTarget* renderTarget,Color color = Color::blue);
	//レンダーターゲットをすべて変更し、画面をクリアする
	void ReSetRenderTarget();
	//レンダーターゲットを変更するだけ
	//第一引数：DWORD 何番目のレンダーターゲットに設定するか,第ニ引数：RenderTarget*　設定するレンダーターゲット
	void SetRenderTarget(DWORD RenderTargetIndex, RenderTarget* renderTarget);
	//第一引数：DWORD 何番目のレンダーターゲットに設定するか,int listの何番目をレンダーターゲットに設定するか
	void SetRenderTarget(DWORD RenderTargetIndex, int idx);
	//レンダーターゲットから外す
	//第一引数：DWORD　外したいレンダーターゲットの添え字
	void RemoveRenderTarget(DWORD RenderTargetIndex);
	//変更したレンダーターゲットをバックバッファーに戻す。
	void BeforeRenderTarget();
	//受け取った引数にﾚﾝﾀﾞｰﾀｰｹﾞｯﾄの画像をコピー
	//void CopyTexture(DWORD Idx,TEXTURE* tex);

	//レンダーターゲットのテクスチャ取得
	TEXTURE* GetRenderTargetTexture(DWORD idx);
	//レンダーターゲット取得
	RenderTarget* GetRenderTarget(DWORD idx);

	static RenderTargetManager* Instance()
	{
		if(instance == nullptr)
		{
			instance = new RenderTargetManager();
		}
		return instance;
	}
private:
	//バックバッファー(最初のレンダーターゲット)保持用
	RenderTarget* backBuffer;
	//レンダーターゲットたち
	vector<RenderTarget*> renderTargerList;

	//レンダーターゲットの数
	int RenderTargetNum;

	static RenderTargetManager* instance;
};