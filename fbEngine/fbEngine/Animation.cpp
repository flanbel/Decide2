#include "Animation.h"

void Animation::Initialize(ID3DXAnimationController* anim)
{
	_AnimController = anim;
	_NumAnimSet = _AnimController->GetMaxNumAnimationSets();
	_NumMaxTracks = _AnimController->GetMaxNumTracks();
	_BlendRateTable.reset(new float[_NumMaxTracks]);
	_AnimationSets.reset(new ID3DXAnimationSet*[_NumAnimSet]);
	_EndTime.reset(new double[_NumAnimSet]);
	FOR(i,_NumAnimSet)
	{
		_EndTime[i] = -1.0f;
	}
	FOR(i, _NumMaxTracks)
	{
		_BlendRateTable[i] = 1.0f;
		//各トラックのローカルタイムを0に初期化。
		SetLocalAnimationTime(i, 0.0f);
	}
	//アニメーションセットを初期化。
	FOR(i, _NumAnimSet)
	{
		//アニメーションセットを取得して配列に格納
		_AnimController->GetAnimationSet(i, &_AnimationSets[i]);
	}
}

void Animation::Awake()
{
	_AnimController = nullptr;
	_NumAnimSet = 0;
	_IsBlending = false;
	_IsInterpolate = false;
	_NumMaxTracks = 0;
	_InterpolateTimer = 0.0f;
	_InterpolateEndTime = 0.0f;
	_CurrentTrackNo = 0;
	_PlaySpeed = 1.0f;
}

void Animation::PlayAnimation(const int& animationSetIndex)
{
	if (animationSetIndex < _NumAnimSet) {
		if (_AnimController) {
			//再生開始
			_IsPlaying = true;
			_CurrentFrame = 0;
			_CurrentAnimationSetNo = animationSetIndex;
			_CurrentTrackNo = 0;

			//0番目以外のトラックは無効にする。
			FOR(i, _NumMaxTracks)
			{
				if (i == _CurrentTrackNo)
				{
					_AnimController->SetTrackEnable(i, TRUE);
				}
				else
				{
					_AnimController->SetTrackEnable(i, FALSE);
				}
			}
			//トラックにアニメーションセット
			_AnimController->SetTrackAnimationSet(_CurrentTrackNo, _AnimationSets[(_NumAnimSet - 1) - _CurrentAnimationSetNo]);
			SetLocalAnimationTime(0, 0.0f);
		}
	}
	else {
		//TK_LOG("warning!!! animationSetIndex is over range!!!!!");
	}
}

void Animation::PlayAnimation(int animationSetIndex, float Timer, int lnum)
{
	if (animationSetIndex < _NumAnimSet) {
		if (_AnimController) {
			//ループ数設定
			_LoopNum = lnum;
			_LoopCount = 0;
			//再生開始
			_IsPlaying = true;
			_CurrentFrame = 0;
			//現在再生中のアニメーションセットNo
			_CurrentAnimationSetNo = animationSetIndex;
			//補間開始の印。
			_IsInterpolate = true;
			//補完時間初期化
			_InterpolateTimer = 0.0f;
			//補完終了時間終了
			_InterpolateEndTime = Timer;
			//トラックNo変更
			_CurrentTrackNo = (_CurrentTrackNo + 1) % _NumMaxTracks;
			//トラックを有効にする。
			_AnimController->SetTrackEnable(_CurrentTrackNo, TRUE);
			//トラックにアニメーションセット
			_AnimController->SetTrackAnimationSet(_CurrentTrackNo, _AnimationSets[(_NumAnimSet - 1) - _CurrentAnimationSetNo]);
			SetLocalAnimationTime(_CurrentTrackNo,0.0f);
		}
	}
	else {
		//TK_LOG("warning!!! animationSetIndex is over range!!!!!");
	}
}

void Animation::Update()
{
	//指定されたループ数ないかどうか
	if (_IsPlaying)
	{
		//nullチェック
		if (_AnimController == nullptr)
			return;

		//デルタタイム
		double delta = Time::DeltaTime() * (double)_PlaySpeed;

		//補完するよ。
		if (_IsInterpolate) {
			//補間中。
			_InterpolateTimer += delta;
			float weight = 0.0f;
			if (_InterpolateTimer > _InterpolateEndTime) {
				//補間終了。
				_IsInterpolate = false;
				weight = 1.0f;
				_AnimController->SetTrackWeight(_CurrentTrackNo, weight);
				//現在のトラック以外を無効にする。
				FOR(i, _NumMaxTracks){
					if (i != _CurrentTrackNo) {
						_AnimController->SetTrackEnable(i, FALSE);
					}
				}
			}
			//補完中
			else {
				//割合を設定
				weight = _InterpolateTimer / _InterpolateEndTime;
				float invWeight = 1.0f - weight;
				//ウェイトを設定していく。
				for (int i = 0; i < _NumMaxTracks; i++) {
					if (i != _CurrentTrackNo) {
						_AnimController->SetTrackWeight(i, _BlendRateTable[i] * invWeight);
					}
					else {
						_AnimController->SetTrackWeight(i, weight);
					}
				}
			}
		}

		//フレームを増加させる
		_CurrentFrame++;
		//現在のトラックのアニメーションセット取得
		LPD3DXANIMATIONSET aniset;
		//設定されているトラックからアニメーションセット取得
		_AnimController->GetTrackAnimationSet(_CurrentTrackNo, &aniset);

		//アニメーションの終了時間設定
		//エンドタイムが指定されているのならそちらを優先
		double endtime = _EndTime[_CurrentAnimationSetNo] > 0.0f ? _EndTime[_CurrentAnimationSetNo] : aniset->GetPeriod();

		//ローカルタイムに加算
		_LocalAnimationTime += delta;
		//グローバルタイムに加算
		_AnimController->AdvanceTime(delta, NULL);
		//割合を計算
		_TimeRatio = min(1.0f, _LocalAnimationTime / endtime);

		SetLocalAnimationTime(_CurrentTrackNo, _LocalAnimationTime);
		FOR(i, _NumMaxTracks)
		{
			D3DXTRACK_DESC desc;
			_AnimController->GetTrackDesc(i, &desc);
			int a = 0;
		}

		//アニメーション終了時間を超えた。
		if (endtime <= _LocalAnimationTime) {
			//経過したフレーム初期化
			_CurrentFrame = 0;
			//ループ数増加
			_LoopCount++;

			//アニメーション時間をリセット
			SetLocalAnimationTime(_CurrentTrackNo, _LocalAnimationTime - endtime);

			if (_LoopNum != -1 &&		//無限ループではない
				_LoopCount >= _LoopNum)	//カウントが指定した数以上になった
			{
				//アニメーション終了
				_IsPlaying = false;
				//最後の方で止める。
				//SetLocalAnimationTime(_CurrentTrackNo, endtime - 0.001f);
			}
		}
	}
}