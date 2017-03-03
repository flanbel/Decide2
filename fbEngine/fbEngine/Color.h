#pragma once

//カラー
class Color {
public:
	union {
		struct { float r, g, b, a; };
		float color[4];
	};
	//赤
	static const Color red;
	//緑
	static const Color green;
	//青
	static const Color blue;
	//黄色
	static const Color yellow;
	//水
	static const Color water;
	//紫
	static const Color purple;
	//白
	static const Color white;
	//黒
	static const Color black;
	//無
	static const Color zero;
public:
	operator D3DXCOLOR() { return static_cast<D3DXCOLOR>(*this); }
	operator LPVOID() { return static_cast<LPVOID>(this); }

	Color()
	{
		this->r = this->g = this->b = this->a = 0;
	}
	/*!
	* @brief	コンストラクタ。
	*/
	Color(float r, float g, float b,float a)
	{
		Set(r, g, b, a);
	}
	//D3DXCOLORをColorに変換
	Color(D3DXCOLOR& c)
	{
		this->r = c.r;
		this->g = c.g;
		this->b = c.b;
		this->a = c.a;
	}
	void Set(float r,float g,float b,float a)
	{
		this->r = r;
		this->g = g;
		this->b = b;
		this->a = a;
	}
	//ColorをD3DXVECTOR3に変換コピーする
	void CopyFrom(D3DXCOLOR& c) const
	{
		c.r = r;
		c.g = g;
		c.b = b;
		c.a = a;
	}
};