#pragma once
#include <fstream>
#include <string>
#include <iostream>

//いろいろ便利な関数を作ろう
namespace Support
{
	//文字列からハッシュ値を求める(簡易)
	extern int MakeHash(const char * s);
	//整数を文字列に
	extern void ToString(const int& dnum, char* s);
	//整数を文字列(wchar)に
	extern void ToString(const int& dnum, wchar_t* s);
	//小数を文字列に
	extern void ToString(const float& fnum, wchar_t * s, const int& decimal = 1);
	//CSVファイルを読み込む(未実装)
	extern void ReadCSV(const char * filename);
}