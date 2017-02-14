#include "Support.h"

//文字列の長さを求める
int Length(char* s)
{
	int count = 0;

	while (*s != '\0')
	{
		s++;
		count++;
	}
	//終端文字分＋
	count += 1;
	return count;
}

int MakeHash(char * s)
{
	int hash = 0;
	while (*s != '\0')
	{
		//加算していく
		hash = hash * 37 + (*s);
		s++;
	}
	return hash;
}

void InttoString(int dnum, char* s)
{
	list<char> l;
	do
	{
		//文字コード変換
		char d = (dnum % 10) + '0';
		//登録
		l.push_back(d);
		//次の桁へ
		dnum /= 10;
	} while (dnum != 0);

	//逆から
	list<char>::reverse_iterator it = l.rbegin();
	short i = 0;
	while(it != l.rend())
	{
		//前から取り出す
		s[i] = (*it);
		it++;
		i++;
	}
	//終端文字
	s[i] = '\0';
}

void InttoString(int dnum, wchar_t* s)
{
	list<char> l;
	do
	{
		//文字コード変換
		char d = (dnum % 10) + '0';
		//登録
		l.push_back(d);
		//次の桁へ
		dnum /= 10;
	} while (dnum != 0);

	//逆から
	list<char>::reverse_iterator it = l.rbegin();
	short i = 0;
	while (it != l.rend())
	{
		//前から取り出す
		s[i] = (*it);
		it++;
		i++;
	}
	//終端文字
	s[i] = '\0';
}

void FloattoString(float fnum, wchar_t * s,int decimal)
{
	//繰り上げ
	int num = (int)(fnum * pow(10, decimal));
	list<char> l;
	do
	{
		//文字コード変換
		char d = (num % 10) + '0';
		//登録
		l.push_back(d);
		//次の桁へ
		num /= 10;
	} while (num != 0);

	//逆から
	list<char>::reverse_iterator it = l.rbegin();
	short i = 0;
	short size = l.size();
	while (it != l.rend())
	{
		if (size - i == decimal)
		{
			s[i] = '.';
			i++;
		}
		//前から取り出す
		s[i] = (*it);
		it++;
		i++;
	}
	//終端文字
	s[i] = '\0';
}

#include <fstream>
#include <string>

#include <iostream>

using namespace std;
//未実装
void ReadCSV(char * filename)
{
	ifstream ifs("Asset//Data//tets.csv");
	string buf;
	vector<vector<string>> Vec;
	int line = 0;
	//読み込み
	while (ifs && getline(ifs, buf))
	{
		string moji = "";
		vector<string> Vs;
		//コンマを探す
		for (short idx = 0,i = 0; idx < (int)buf.length(); idx++)
		{
			if(buf[idx] == ',')
			{
				Vs.push_back(moji);
				i++;
				moji = "";
			}
			else
			{
				moji += buf[idx];
			}
			
		}
		//文字列追加
		Vec.push_back(Vs);
		line++;
	}

	return;
}