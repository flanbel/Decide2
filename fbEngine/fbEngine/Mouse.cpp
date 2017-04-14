#include "stdafx.h"
#include "Mouse.h"

HRESULT Mouse::Initialize(LPDIRECTINPUT8 Dinput)
{
	//デバイス作成
	if (FAILED(Dinput->CreateDevice(
		GUID_SysMouse,	// マウスを受け付ける
		&mouseDevice,	// IDirectInputDevice8ポインタ
		NULL)))			// 使わない
	{
		return false;
	}

	//受け取る構造体のフォーマットを設定
	if (FAILED(mouseDevice->SetDataFormat(&c_dfDIMouse2)))
	{
		return false;
	}


	// 入力デバイスへのアクセス権を取得
	mouseDevice->Acquire();

	HWND hWnd = FindWindow("DECIDE", NULL);
	g_MainWindow;
	mouseDevice->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);

	// デバイスの設定    
	DIPROPDWORD diprop;
	diprop.diph.dwSize = sizeof(DIPROPDWORD);
	diprop.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	diprop.diph.dwObj = 0;
	diprop.diph.dwHow = DIPH_DEVICE;
	// 相対値モードで設定（絶対値はDIPROPAXISMODE_ABS）
	//相対値は前フレームからの相対値(移動量？)
	//絶対値は相対値の累計
	diprop.dwData = DIPROPAXISMODE_REL;
	mouseDevice->SetProperty(DIPROP_AXISMODE, &diprop.diph);

	// 入力制御開始
	mouseDevice->Acquire();
			
	return D3D_OK;
}

void Mouse::Update()
{
	// 値の初期化    
	ZeroMemory(&mouseState, sizeof(mouseState));
	// 値の更新 
	if (FAILED(mouseDevice->GetDeviceState(sizeof(DIMOUSESTATE2), &mouseState)))
	{
		//失敗したのでもう一度受け取れるようにする
		mouseDevice->Acquire();
		mouseDevice->GetDeviceState(sizeof(DIMOUSESTATE2), &mouseState);
	}
}

int Mouse::GetValue(MouseInE m)
{
	int value = false;
	switch (m)
	{
	case L_CLICK:
		value = (mouseState.rgbButtons[0] & 0x80);
		break;
	case R_CLICK:
		value = (mouseState.rgbButtons[1] & 0x80);
		break;
	case WHEEL_PUSH:
		value = (mouseState.rgbButtons[2] & 0x80);
		break;
	case X:
		value = mouseState.lX;
		break;
	case Y:
		value = mouseState.lY;
		break;
	case WHEEL_ROLL:
		value = mouseState.lZ;
		break;
	default:
		break;
	}
	return value;
}

Vector2 Mouse::GetCursorPosOnScreen()
{
	POINT pos;
	GetCursorPos(&pos);
	return Vector2(pos.x, pos.y);
}

Vector2 Mouse::GetCursorPosOnWindow(HWND hwnd)
{
	POINT pos;
	//絶対値を取得
	GetCursorPos(&pos);
	//相対的な位置を取得
	ScreenToClient(hwnd, &pos);
	return Vector2(pos.x, pos.y);
}
