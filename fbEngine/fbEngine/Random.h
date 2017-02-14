#pragma once
//乱数生成関数
class Random
{
public:
	//範囲内の値を返す
	static int Range(int min, int max);
	//0.0~1.0の間の値を返す
	static double RandDouble();
	//乱数を返す
	static int RandInt();
	//初期シード設定
	static void Seed(int seed)
	{
		srand(seed);
	}
};