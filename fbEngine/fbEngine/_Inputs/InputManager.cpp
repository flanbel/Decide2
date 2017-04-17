#include "InputManager.h"

InputManager* InputManager::_Instance = nullptr;

HRESULT InputManager::Initialize()
{
	if (FAILED(DirectInput8Create(
		GetModuleHandle(NULL),	// インスタンス ハンドル
		DIRECTINPUT_VERSION,	// DirectInputのバージョン
		IID_IDirectInput8,		// 取得インターフェイスのタイプ
		(VOID**)&_Dinput,		// インターフェイスポインタ
		NULL)))					// 使わない
	{
		return false;
	}
	//キーボード初期化
	_KeyBoard = new KeyBoard();
	if (FAILED(_KeyBoard->Initialize(_Dinput)))
	{
		SAFE_DELETE(_KeyBoard);
		return false;
	}

	//マウス初期化
	_Mouse = new Mouse();
	if (FAILED(_Mouse->Initialize(_Dinput)))
	{
		SAFE_DELETE(_Mouse);
		return false;
	}

	FOR(idx,XBOX_CONTROLLER_NUM)
	{
		_Xinput[idx] = new XInput;
		_Xinput[idx]->Initialize(idx);
	}

	return D3D_OK;
}

void InputManager::Update()
{
	_KeyBoard->Update();
	_Mouse->Update();
	FOR(idx, XBOX_CONTROLLER_NUM)
	{
		_Xinput[idx]->Update();
	}
}

KeyBoard * InputManager::GetKeyBoard()
{
	return _KeyBoard;
}

Mouse * InputManager::GetMouse()
{
	return _Mouse;
}

XInput * InputManager::GetXInput(int idx)
{
	return _Xinput[idx];
}

bool InputManager::IsPushButtonAll(int in)
{
	FOR(idx,XBOX_CONTROLLER_NUM)
	{
		if (GetXInput(idx)->IsPushButton(in))
		{
			return true;
		}
	}
	return false;
}

InputManager* InputManager::Instance()
{
	if(_Instance == nullptr)
	{
		_Instance = new InputManager();
	}

	return _Instance;
}
