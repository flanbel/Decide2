#pragma once
#pragma comment(lib, "winmm.lib ")
#include <time.h>
#include <mmsystem.h>

//時間クラス
class Time
{
public:
	Time(){};
	static void Update();
	//デルタタイム取得
	static const double& DeltaTime()
	{
		return deltaTime;
	}
	//FPS(仮)取得
	static const float& Fps()
	{
		return fps;
	}
private:
	//前回のアップデートと今のアップデートとの時差を格納(ミリ秒単位)
	static double deltaTime;
	//前回の時間格納
	static DWORD  beforeTime;
	//フレームパーセカンド(1秒間にでるフレームの数)
	static float fps;

	//フレーム数
	static float fream;
	static double time;
};