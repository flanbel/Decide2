#pragma once
//入力デバイスの管理とかしてるよ

#include "KeyBoard .h"
#include "Mouse.h"
#include "XInput.h"

//Xboxコントローラーの数(ここに書かなくてもいいかも。)
#define XBOX_CONTROLLER_NUM 4

class InputManager
{
public:
	HRESULT Initialize();
	void Update();
	KeyBoard* GetKeyBoard();
	Mouse* GetMouse();
	XInput* GetXInput(int idx);
	//全Xboxコントローラー内のどれかで指定したボタンが押されているか？
	bool IsPushButtonAll(int in);
	static InputManager* Instance();
private:
	// DirectInput
	LPDIRECTINPUT8 _Dinput;
	//キーボードの入力
	KeyBoard* _KeyBoard;
	//マウスの入力
	Mouse* _Mouse;
	//Xboxゲームパッドの入力
	XInput* _Xinput[XBOX_CONTROLLER_NUM];
	static InputManager* _Instance;
};

#define KeyBoardInput INSTANCE(InputManager)->GetKeyBoard()
#define MouseInput INSTANCE(InputManager)->GetMouse()
#define XboxInput(idx) INSTANCE(InputManager)->GetXInput(idx)