#pragma once

//キーボードからの入力受付
class KeyBoard
{
public:
	HRESULT Initialize(LPDIRECTINPUT8 Dinput);
	void Update();

	//keyにはDIK_??を使うとよい
	//一瞬
	bool isPush(int key);
	//継続的
	bool isPressed(int key);
private:
	// DirectInputデバイス
	LPDIRECTINPUTDEVICE8 keyDevice;
	//キーボードの情報が格納される
	BYTE now[256];
	//前のフレームの状態を格納
	BYTE old[256];
};