#include "Support.h"

namespace Support
{
	int MakeHash(const char * s)
	{
		int hash = 0;
		while (*s != '\0')
		{
			//加算していく
			hash = hash * 37 + (*s);
			//次の文字に
			s++;
		}
		return hash;
	}

	void ToString(const int& dnum, char* s)
	{
		list<char> l;
		//結局コピーしちゃった
		int num = dnum;
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

	void ToString(const int& dnum, wchar_t* s)
	{
		list<char> l;
		//結局コピーしちゃった
		int num = dnum;
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

	void ToString(const float& fnum, wchar_t * s, const int& decimal)
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
		size_t size = l.size();
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

	void ReadCSV(char * filename)
	{
		std::ifstream ifs("Asset//Data//tets.csv");
		string buf;
		vector<vector<string>> Vec;
		int line = 0;
		//読み込み
		while (ifs && getline(ifs, buf))
		{
			string moji = "";
			vector<string> Vs;
			//コンマを探す
			for (short idx = 0, i = 0; idx < (int)buf.length(); idx++)
			{
				if (buf[idx] == ',')
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
}




