#include "TextureAnimator.h"

TextureAnimator::TextureAnimator(GameObject * g, Transform * t):
	Component(g, t, typeid(this).name())
{

}

void TextureAnimator::Awake()
{
	_isPlaying = false;
	_End = false;
	_LoopNum = 0;
	_LoopCount = 0;
	_Division = Vector2(0, 0);
	_UV = Vector4(0, 1, 0, 1);
	_NowAnimNum = 0;
	_MaxAnimNum = 0;
	_PlaybackTime = 0.0f;
	_TimePerSheet = 0.0f;
	_Timer = 0.0f;
}

void TextureAnimator::Update()
{
	//アニメーション再生中 &&
	//設定されている。
	if (_isPlaying && 0 < _MaxAnimNum)
	{
		_Timer += Time::DeltaTime();
		//再生時間を超えた
		if (_Timer > _TimePerSheet)
		{
			//何枚進んだか計算
			int add = _Timer / _TimePerSheet;
			//残った時間引継ぎ
			_Timer -= _TimePerSheet * add;
			//今の数
			int NowTmp = _NowAnimNum + add;
			//カウント進める？
			_LoopCount += NowTmp / (_MaxAnimNum-1);

			if (NowTmp < _MaxAnimNum ||	//最大数より小さい
				_LoopNum < 0 ||			//∞ループ
				_LoopCount < _LoopNum)	//loopする
			{
				//枚数を進める
				_NowAnimNum = NowTmp % _MaxAnimNum;
				//UV計算
				_CalcUV();
			}
			else
			{
				_isPlaying = false;
				_End = true;
				_NowAnimNum = 0;
				_UV = Vector4(0, 1, 0, 1);
			}
		}
	}
}

void TextureAnimator::Play(const float& time, const int& loop, const int& start)
{
	_isPlaying = true;
	_End = false;
	_LoopNum = loop;
	_LoopCount = 0;
	_NowAnimNum = min(start, _MaxAnimNum - 1);
	//UV計算
	_CalcUV();

	_PlaybackTime = time;
	_TimePerSheet = _PlaybackTime / (float)_MaxAnimNum;
	_Timer = 0.0f;
}

void TextureAnimator::Pause()
{
	_isPlaying = false;
}

void TextureAnimator::SetSplit(const int& X, const int& Y, const int& max)
{
	_Division.x = X;
	_Division.y = Y;
	//1区切りあたりの幅と高さ
	_WidthX = 1.0 / (double)_Division.x;
	_WidthY = 1.0 / (double)_Division.y;
	//アニメーションの最大枚数設定
	_MaxAnimNum = max;
}

const Vector4& TextureAnimator::GetUV()
{
	return _UV;
}

const bool & TextureAnimator::GetEnd()
{
	return _End;
}

void TextureAnimator::_CalcUV()
{
	//縦横の何枚目か
	int nowX, nowY;
	nowX = _NowAnimNum % (int)_Division.x;
	nowY = _NowAnimNum / (int)_Division.x;
	//UV計算
	_UV.x = nowX * _WidthX;
	_UV.y = _UV.x + _WidthX;
	_UV.z = nowY * _WidthY;
	_UV.w = _UV.z + _WidthY;
}
