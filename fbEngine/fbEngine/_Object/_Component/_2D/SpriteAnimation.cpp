#include "SpriteAnimation.h"

SpriteAnimation::SpriteAnimation(GameObject * g, Transform * t):
	Component(g, t, typeid(this).name())
{

}

void SpriteAnimation::Awake()
{
}

void SpriteAnimation::Start()
{
}

void SpriteAnimation::Update()
{
	//アニメーション再生中なら
	if (_Play)
	{
		_Timer += Time::DeltaTime();
		//再生時間を超えた
		if (_Timer > _PlaybackTime)
		{
			//リセット？
			_Timer = 0.0f;
		}
		//時間の経過から現在の枚数を算出
		_NowAnimNum = (_Timer / _PlaybackTime) * _MaxAnimNum;
	}
}

void SpriteAnimation::ImageRender()
{
}

void SpriteAnimation::Play(float time, int start)
{
	//テクスチャが設定されている。
	if (_MaxAnimNum > 0)
	{
		_Play = true;
		_PlaybackTime = time;
		_NowAnimNum = start;
		_Timer = ((float)_NowAnimNum / (float)_MaxAnimNum) * _PlaybackTime;
	}
}

void SpriteAnimation::SetTexture(TEXTURE * tex, int X, int Y)
{
	//アニメーションの最大枚数設定
	_MaxAnimNum = X * Y;
}
