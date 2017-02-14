#pragma once

enum Analog {
	L_TRIGGER,	//左ﾄﾘｶﾞｰ
	R_TRIGGER,	//右ﾄﾘｶﾞｰ
	L_STICKR,	//左スティック右
	L_STICKL,	//左スティック左
	L_STICKU,	//左スティック上
	L_STICKD,	//左スティック下
	R_STICKR,	//右スティック右
	R_STICKL,	//右スティック左
	R_STICKU,	//右スティック上
	R_STICKD,	//右スティック下
};

enum AnalogInput
{
	TRIGGER,	//トリガー
	L_STICK,	//左スティック
	R_STICK,	//右スティック
};

//Xbox360のコントローラー
class XInput
{
public:
	XInput();
	//第一引数：int 添え字(0~3)
	void Initialize(int index);
	//値の更新
	void Update();
	//接続確認
	bool IsConnected();
	//ボタンの瞬間的な押下
	bool isPushButton(int in);
	//ボタンの継続的な押下
	bool IsPressButton(int in);
	//アナログスティック、トリガーが瞬間的に入力されているか
	bool IsPushAnalog(Analog a);
	//トリガーやスティックの値が欲しいときにどうぞ。
	//スティックは-32768 ～ 32767(65536)
	//トリガーは0 ～ 255
	Vector2 GetAnalog(AnalogInput in);
	//モーターを振動させる
	//第一引数：int 右モーターの振動数
	//第二引数：int 左モーターの振動数
	void Vibration(int Rmoter, int Lmoter);
private:
	//丸める
	void Rounding();

	int userIndex;			//コントローラーのナンバー(0~3)
	bool Connect;			//接続されているかどうか
	XINPUT_STATE state;
	XINPUT_STATE before;	//一つ前のステート
};