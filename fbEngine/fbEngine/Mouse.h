#pragma once
//マウスの入力値？
enum MouseInE
{
	L_CLICK = 0,//左クリック
	R_CLICK,	//右クリック
	WHEEL_PUSH,	//ホイールを押したとき
	X = 9,		//マウスのX軸
	Y,			//マウスのY軸
	WHEEL_ROLL	//ホイールの回転
};
//マウスからの入力受付
class Mouse
{
public:
	HRESULT Initialize(LPDIRECTINPUT8 Dinput);
	void Update();
	//値を取得
	int GetValue(MouseInE m);
	Vector2 GetCursorPosition();
private:
	// DirectInputデバイス
	LPDIRECTINPUTDEVICE8 mouseDevice;
	//マウスの情報
	DIMOUSESTATE2 mouseState;
};