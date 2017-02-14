#pragma once

//いろいろ便利な関数を作ろう

//文字列の長さを求める
extern int Length(char* s);
//文字列からハッシュ値を求める(簡易)
extern int MakeHash(char* s);
//整数を文字列に
extern void InttoString(int d,char* s);
//整数を文字列(wchar)に
extern void InttoString(int d, wchar_t* s);
//小数を文字列に
extern void FloattoString(float f, wchar_t* s,int decimal = 1);
//CSVファイルを読み込む
extern void ReadCSV(char* filename);