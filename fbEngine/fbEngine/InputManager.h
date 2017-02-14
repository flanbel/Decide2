#pragma once
//入力デバイスの管理とかしてるよ

#include "KeyBoard .h"
#include "Mouse.h"
#include "XInput.h"


class InputManager
{
public:
	HRESULT Initialize();
	void Update();
	KeyBoard* GetKeyBoard();
	Mouse* GetMouse();
	XInput* GetXInput(int idx);
	static InputManager* Instance();
private:
	// DirectInput
	LPDIRECTINPUT8 Dinput;
	//キーボードの入力
	KeyBoard* keyBoard;
	//マウスの入力
	Mouse* mouse;
	//Xboxゲームパッドの入力(4つ分)
	XInput* xinput[4];
	static InputManager* instance;
};

#define KeyBoardInput INSTANCE(InputManager)->GetKeyBoard()
#define MouseInput INSTANCE(InputManager)->GetMouse()
#define XboxInput(idx) INSTANCE(InputManager)->GetXInput(idx)