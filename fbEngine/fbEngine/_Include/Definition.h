#pragma once
//便利なdefine定義

//インスタンスリリース
#define SAFE_RELEASE(p)			{ if(p) { (p)->Release(); (p) = NULL; } }
//インスタンス削除
#define SAFE_DELETE(p)			{ if(p!=nullptr) { delete (p);     (p) = nullptr; } }
//インスタンス配列削除
#define SAFE_DELETE_ARRAY(p)	{ if(p!=nullptr) { delete[] (p);   (p) = nullptr; } }

//変数countを使ってnum回繰り返す
#define FOR(count,num) for(int count = 0;count < (int)num;count++)
//1をnビットずらしたものを返す(マスクとかで使うといいかも)
#define BIT(n) ( 1 << (n) )
//シングルトン取得
#define INSTANCE(p) p::Instance()
//受け取った値を文字列に変換(enumや変数名も可能。)
#define STRING(var) #var