#include "XInput.h"

XInput::XInput()
{

}

void XInput::Initialize(int index)
{
	_UserIndex = index;
	ZeroMemory(&_State, sizeof(XINPUT_STATE)); // 初期化
	ZeroMemory(&_BeforeState, sizeof(XINPUT_STATE)); // 初期化
}

void XInput::Update()
{
	//前のステート保存
	_BeforeState = _State;
	//ステート取得
	DWORD er = XInputGetState(_UserIndex, &_State);
	//デッドゾーンなら値を丸める
	_Rounding();
	//接続できているかどうか
	if (er == ERROR_DEVICE_NOT_CONNECTED)
	{
		_IsConnect = false;
	}
	else if (er == ERROR_SUCCESS)
	{
		_IsConnect = true;
	}
}

bool XInput::IsConnected()
{
	return _IsConnect;
}

bool XInput::IsPushButton(int gamepad)
{
	if (!(_BeforeState.Gamepad.wButtons & gamepad) &&		//前フレームに押されていない
		(_State.Gamepad.wButtons & gamepad))				//今押されている
	{
		return true;
	}
	return false;
}

bool XInput::IsPressButton(int gamepad)
{
	//論理積をとる
	return ((_State.Gamepad.wButtons & gamepad) > 0);
}

bool XInput::IsPushAnalog(AnalogE a)
{
	//接続確認
	if (!IsConnected())
	{
		return false;
	}

	switch (a)
	{
	case L_TRIGGER:
		if (_BeforeState.Gamepad.bLeftTrigger == 0 &&		//押されていない
			_State.Gamepad.bLeftTrigger > 0)				//今押されている
			return true;
		break;
	case R_TRIGGER:
		if (_BeforeState.Gamepad.bRightTrigger == 0 &&		//押されていない
			_State.Gamepad.bRightTrigger > 0)				//今押されている
			return true;
		break;
	case L_STICKR:
		if (_BeforeState.Gamepad.sThumbLX == 0 &&		//押されていない
			_State.Gamepad.sThumbLX > 0)				//今押されている
			return true;
		break;
	case L_STICKL:
		if (_BeforeState.Gamepad.sThumbLX == 0 &&		//押されていない
			_State.Gamepad.sThumbLX < 0)				//今押されている
			return true;
		break;
	case L_STICKU:
		if (_BeforeState.Gamepad.sThumbLY == 0 &&		//押されていない
			_State.Gamepad.sThumbLY > 0)				//今押されている
			return true;
		break;
	case L_STICKD:
		if (_BeforeState.Gamepad.sThumbLY == 0 &&		//押されていない
			_State.Gamepad.sThumbLY < 0)				//今押されている
			return true;
		break;
	case R_STICKR:
		if (_BeforeState.Gamepad.sThumbRX == 0 &&		//押されていない
			_State.Gamepad.sThumbRX > 0)				//今押されている
			return true;
		break;
	case R_STICKL:
		if (_BeforeState.Gamepad.sThumbRX == 0 &&		//押されていない
			_State.Gamepad.sThumbRX < 0)				//今押されている
			return true;
		break;
	case R_STICKU:
		if (_BeforeState.Gamepad.sThumbRY == 0 &&		//押されていない
			_State.Gamepad.sThumbRY > 0)				//今押されている
			return true;
		break;
	case R_STICKD:
		if (_BeforeState.Gamepad.sThumbRY == 0 &&		//押されていない
			_State.Gamepad.sThumbRY < 0)				//今押されている
			return true;
		break;
	default:
		break;
	}

	return false;
}

Vector2 XInput::GetAnalog(AnalogInputE in)
{
	Vector2 value;
	switch (in)
	{
	case TRIGGER:
		value.x = _State.Gamepad.bRightTrigger;
		value.y = _State.Gamepad.bLeftTrigger;
		break;
	case L_STICK:
		value.x = _State.Gamepad.sThumbLX;
		value.y = _State.Gamepad.sThumbLY;
		break;
	case R_STICK:
		value.x = _State.Gamepad.sThumbRX;
		value.y = _State.Gamepad.sThumbRY;
		break;
	default:
		break;
	}
	return value;
}
;

void XInput::Vibration(int Rmoter, int Lmoter)
{
	//振動
	XINPUT_VIBRATION vibration;
	//初期化
	ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
	vibration.wRightMotorSpeed = Rmoter; //右モーターの回転数 use any value between 0-65535 here
	vibration.wLeftMotorSpeed = Lmoter; //左モーターの回転数 use any value between 0-65535 here
	//振動セット , エラーチェック
	if(XInputSetState(_UserIndex, &vibration) == ERROR_DEVICE_NOT_CONNECTED)
	{
		//接続されてない
		_IsConnect = false;
	}
}

void XInput::_Rounding()
{
	/*const int LowerLimit = 100;
	const int UpperLimit = 65535;*/

	if (-XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE < _State.Gamepad.sThumbLY &&
		_State.Gamepad.sThumbLY < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
	{
		_State.Gamepad.sThumbLY = 0;
	}

	if (-XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE < _State.Gamepad.sThumbLX &&
		_State.Gamepad.sThumbLX < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
	{
		_State.Gamepad.sThumbLX = 0;
	}

	if (-XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE < _State.Gamepad.sThumbRY &&
		_State.Gamepad.sThumbRY <  XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
	{ _State.Gamepad.sThumbRY = 0; }

	if (-XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE < _State.Gamepad.sThumbRX &&
		_State.Gamepad.sThumbRX <  XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
	{ _State.Gamepad.sThumbRX = 0; }
}