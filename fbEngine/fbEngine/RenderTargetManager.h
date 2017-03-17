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
	TEXTURE *texture;
	RenderTarget();
	void Create(Vector2 size,_D3DFORMAT colorfmt = D3DFMT_A8R8G8B8);
};

//レンダーターゲットの添え字
enum RTIdxE
{
	SHADOWDEPTH = 0
};

//レンダリングターゲット管理クラス
class RenderTargetManager
{
public:
	//レンダーターゲットを保持
	RenderTargetManager();

	//レンダーターゲットをすべて変更し、画面をクリアする
	void ReSetRT();
	//レンダーターゲットを変更し、画面をクリアする
	//第一引数：DWORD 何番目のレンダーターゲットに設定するか,第ニ引数：RenderTarget*　設定するレンダーターゲット
	void ReSetRT(DWORD RenderTargetIndex, RenderTarget* renderTarget,Color color = Color::blue);
	//レンダーターゲットを変更するだけ
	//第一引数：DWORD 何番目のレンダーターゲットに設定するか,第ニ引数：RenderTarget*　設定するレンダーターゲット
	void SetRT(DWORD RenderTargetIndex, RenderTarget* renderTarget);
	//第一引数：DWORD 何番目のレンダーターゲットに設定するか,int listの何番目をレンダーターゲットに設定するか
	void SetRT(DWORD RenderTargetIndex, int idx);
	//レンダーターゲットから外す
	//第一引数：DWORD　外したいレンダーターゲットの添え字
	void RemoveRT(DWORD RenderTargetIndex);
	//変更したレンダーターゲットをバックバッファーに戻す。
	void BeforeRT();
	//受け取った引数にﾚﾝﾀﾞｰﾀｰｹﾞｯﾄの画像をコピー
	//void CopyTexture(DWORD Idx,TEXTURE* tex);

	//レンダーターゲットのテクスチャ取得
	TEXTURE* GetRTTextureFromList(DWORD idx);
	//レンダーターゲット取得
	RenderTarget* GetRTFromList(DWORD idx);

	static RenderTargetManager* Instance()
	{
		if(_Instance == nullptr)
		{
			_Instance = new RenderTargetManager();
		}
		return _Instance;
	}
private:
	//バックバッファー(最初のレンダーターゲット)保持用
	RenderTarget* _BackBuffer;
	//レンダーターゲットたち
	vector<RenderTarget*> _RTList;

	//レンダーターゲットの数
	int _MaxRTNum;

	static RenderTargetManager* _Instance;
};