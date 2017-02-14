#include "Time.h"


//これは,timeGetTimeを使うver
//他には高分解能カウンタと高分解能周波数を使った,より精度の高い方法もある(KBCFreamworkでみた)

double Time::deltaTime = 0.0f;
//パソコンを起動して何ミリ秒経過したか取得
DWORD Time::beforeTime = timeGetTime();
float Time::fps = 0;

float Time::fream = 0;
double Time::time = 0;

void Time::Update()
{
	//今の時間を取得
	DWORD afterTime = timeGetTime();
	//前回と今の差をデルタタイムに設定(ミリ秒を秒に変換)
	deltaTime = (afterTime - beforeTime) / 1000.0f;
	//beforeを更新
	beforeTime = afterTime;
	
	time += deltaTime;
	fream++;

	//1秒経過したなら
	if (time >= 1.0f)
	{
		//超過分
		double over = (double)1.0f / time;
		//FPS更新
		fps = (float)(fream * over);

		time = 0;
		fream = 0;
	}
}