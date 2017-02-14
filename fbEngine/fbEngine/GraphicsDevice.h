#pragma once

//Direct3Dデバイス・オブジェクトを管理するクラス
class CGraphicsDevice
{
public:
	CGraphicsDevice(void);
	HRESULT InitD3d(HWND);	//Direct3Dオブジェクトおよびデバイス初期化
	void FreeDX();		//Direct3Dデバイス・オブジェクト解放
	//LPDIRECT3DDEVICE9へのキャストに関するオペレータ演算子
	operator LPDIRECT3DDEVICE9() const { return this->m_pDevice; }
private:
	LPDIRECT3D9 m_pD3d;			//DIRECT3Dオブジェクト
	LPDIRECT3DDEVICE9 m_pDevice;	//Direct3Dデバイス
};

//グローバル関数を定義してどこかにある実体を返す(ここでいうとcGraphicsDeviceクラスのインスタンスを返す)
inline CGraphicsDevice& graphicsDevice()
{
	extern CGraphicsDevice g_graphicsDevice;
	return g_graphicsDevice;
}