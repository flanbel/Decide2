#include "XInput.h"

XInput::XInput()
{

}

void XInput::Initialize(int index)
{
	userIndex = index;
	ZeroMemory(&state, sizeof(XINPUT_STATE)); // 初期化
	ZeroMemory(&before, sizeof(XINPUT_STATE)); // 初期化
}

void XInput::Update()
{
	before = state;
	//ステート取得
	DWORD er = XInputGetState(userIndex, &state);
	//デッドゾーンなら値を丸める
	Rounding();
	//接続できているかどうか
	if (er == ERROR_DEVICE_NOT_CONNECTED)
	{
		Connect = false;
	}
	else if (er == ERROR_SUCCESS)
	{
		Connect = true;
	}
}

bool XInput::IsConnected()
{
	return Connect;
}

bool XInput::isPushButton(int gamepad)
{
	if (!(before.Gamepad.wButtons & gamepad) &&		//前フレームに押されていない
		(state.Gamepad.wButtons & gamepad))				//今押されている
	{
		return true;
	}
	return false;
}

bool XInput::IsPressButton(int gamepad)
{
	if (state.Gamepad.wButtons & gamepad)				//今押されている
	{
		return true;
	}
	return false;
}

bool XInput::IsPushAnalog(Analog a)
{
	//接続確認
	if (!IsConnected())
	{
		return false;
	}

	switch (a)
	{
	case L_TRIGGER:
		if (before.Gamepad.bLeftTrigger == 0 &&		//押されていない
			state.Gamepad.bLeftTrigger > 0)				//今押されている
			return true;
		break;
	case R_TRIGGER:
		if (before.Gamepad.bRightTrigger == 0 &&		//押されていない
			state.Gamepad.bRightTrigger > 0)				//今押されている
			return true;
		break;
	case L_STICKR:
		if (before.Gamepad.sThumbLX == 0 &&		//押されていない
			state.Gamepad.sThumbLX > 0)				//今押されている
			return true;
		break;
	case L_STICKL:
		if (before.Gamepad.sThumbLX == 0 &&		//押されていない
			state.Gamepad.sThumbLX < 0)				//今押されている
			return true;
		break;
	case L_STICKU:
		if (before.Gamepad.sThumbLY == 0 &&		//押されていない
			state.Gamepad.sThumbLY > 0)				//今押されている
			return true;
		break;
	case L_STICKD:
		if (before.Gamepad.sThumbLY == 0 &&		//押されていない
			state.Gamepad.sThumbLY < 0)				//今押されている
			return true;
		break;
	case R_STICKR:
		if (before.Gamepad.sThumbRX == 0 &&		//押されていない
			state.Gamepad.sThumbRX > 0)				//今押されている
			return true;
		break;
	case R_STICKL:
		if (before.Gamepad.sThumbRX == 0 &&		//押されていない
			state.Gamepad.sThumbRX < 0)				//今押されている
			return true;
		break;
	case R_STICKU:
		if (before.Gamepad.sThumbRY == 0 &&		//押されていない
			state.Gamepad.sThumbRY > 0)				//今押されている
			return true;
		break;
	case R_STICKD:
		if (before.Gamepad.sThumbRY == 0 &&		//押されていない
			state.Gamepad.sThumbRY < 0)				//今押されている
			return true;
		break;
	default:
		break;
	}

	return false;
}

Vector2 XInput::GetAnalog(AnalogInput in)
{
	Vector2 value;
	switch (in)
	{
	case TRIGGER:
		value.x = state.Gamepad.bRightTrigger;
		value.y = state.Gamepad.bLeftTrigger;
		break;
	case L_STICK:
		value.x = state.Gamepad.sThumbLX;
		value.y = state.Gamepad.sThumbLY;
		break;
	case R_STICK:
		value.x = state.Gamepad.sThumbRX;
		value.y = state.Gamepad.sThumbRY;
		break;
	default:
		break;
	}
	return value;
}
;

void XInput::Vibration(int Rmoter, int Lmoter)
{
	DWORD error;
	//振動
	XINPUT_VIBRATION vibration;
	ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
	vibration.wRightMotorSpeed = Rmoter; // use any value between 0-65535 here
	vibration.wLeftMotorSpeed = Lmoter; // use any value between 0-65535 here
	error = XInputSetState(userIndex, &vibration);
}

void XInput::Rounding()
{
	/*const int LowerLimit = 100;
	const int UpperLimit = 65535;*/

	//下限
	if (-XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE < state.Gamepad.sThumbLY && state.Gamepad.sThumbLY <  XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) { state.Gamepad.sThumbLY = 0; }

	if (-XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE < state.Gamepad.sThumbLX &&
		state.Gamepad.sThumbLX <  XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
	{
		state.Gamepad.sThumbLX = 0;
	}

	if (-XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE < state.Gamepad.sThumbRY && state.Gamepad.sThumbRY <  XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) { state.Gamepad.sThumbRY = 0; }

	if (-XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE < state.Gamepad.sThumbRX && state.Gamepad.sThumbRX <  XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) { state.Gamepad.sThumbRX = 0; }
}