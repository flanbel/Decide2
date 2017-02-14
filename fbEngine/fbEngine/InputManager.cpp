#include "InputManager.h"

InputManager* InputManager::instance = nullptr;

HRESULT InputManager::Initialize()
{
	if (FAILED(DirectInput8Create(
		GetModuleHandle(NULL),	// インスタンス ハンドル
		DIRECTINPUT_VERSION,	// DirectInputのバージョン
		IID_IDirectInput8,		// 取得インターフェイスのタイプ
		(VOID**)&Dinput,		// インターフェイスポインタ
		NULL)))					// 使わない
	{
		return false;
	}
	//キーボード初期化
	keyBoard = new KeyBoard();
	if (FAILED(keyBoard->Initialize(Dinput)))
	{
		SAFE_DELETE(keyBoard);
		return false;
	}

	//マウス初期化
	mouse = new Mouse();
	if (FAILED(mouse->Initialize(Dinput)))
	{
		SAFE_DELETE(mouse);
		return false;
	}

	FOR(4)
	{
		xinput[i] = new XInput;
		xinput[i]->Initialize(i);
	}

	return D3D_OK;
}

void InputManager::Update()
{
	keyBoard->Update();
	mouse->Update();
	FOR(4)
		xinput[i]->Update();
}

KeyBoard * InputManager::GetKeyBoard()
{
	return keyBoard;
}

Mouse * InputManager::GetMouse()
{
	return mouse;
}

XInput * InputManager::GetXInput(int idx)
{
	return xinput[idx];
}

InputManager* InputManager::Instance()
{
	if(instance == nullptr)
	{
		instance = new InputManager();
	}

	return instance;
}
