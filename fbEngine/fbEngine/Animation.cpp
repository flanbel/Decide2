#include "Animation.h"

void Animation::Initialize(ID3DXAnimationController* anim)
{
	_AnimController = anim;
	_NumAnimSet = _AnimController->GetMaxNumAnimationSets();
	_NumMaxTracks = _AnimController->GetMaxNumTracks();
	_BlendRateTable.reset(new float[_NumMaxTracks]);
	_AnimationSets.reset(new ID3DXAnimationSet*[_NumAnimSet]);
	for (int i = 0; i < _NumMaxTracks; i++){
		_BlendRateTable[i] = 1.0f;
	}
	//アニメーションセットを初期化。
	for (int i = 0; i < _NumAnimSet; i++) {
		_AnimController->GetAnimationSet(i, &_AnimationSets[i]);
		_AnimController->SetTrackPosition(i, 0.0f);
		_AnimController->AdvanceTime(0, NULL);
	}
}

void Animation::Awake()
{
	_AnimController = nullptr;
	_NumAnimSet = 0;
	_IsBlending = false;
	_IsInterpolate = false;
	_NumMaxTracks = 0;
	_InterpolateTime = 0.0f;
	_InterpolateEndTime = 0.0f;
	_CurrentTrackNo = 0;
}

void Animation::PlayAnimation(const int& animationSetIndex)
{
	if (animationSetIndex < _NumAnimSet) {
		if (_AnimController) {
			//再生開始
			_IsPlaying = true;
			_CurrentFrame = 0;
			//アニメーションのグローバルタイムをリセット
			_AnimController->ResetTime();
			_CurrentAnimationSetNo = animationSetIndex;
			_CurrentTrackNo = 0;
			//0番目以外のトラックは無効にする。
			for (int i = 1; i < _NumMaxTracks; i++) {
				_AnimController->SetTrackEnable(i, FALSE);
			}
			_AnimController->SetTrackWeight(0, 1.0f);
			_AnimController->SetTrackAnimationSet(_CurrentTrackNo, _AnimationSets[(_NumAnimSet - 1) - _CurrentAnimationSetNo]);
			_AnimController->SetTrackEnable(0, TRUE);
			_AnimController->SetTrackPosition(0, 0.0f);
			_AnimController->AdvanceTime(0, NULL);
			SetAnimeSpeed(1.0f);
		}
	}
	else {
		//TK_LOG("warning!!! animationSetIndex is over range!!!!!");
	}
}

void Animation::PlayAnimation(int animationSetIndex, float _InterpolateTime, int lnum)
{
	if (animationSetIndex < _NumAnimSet) {
		if (_AnimController) {
			//ループ数設定
			_LoopNum = lnum;
			_LoopCount = 0;
			//再生開始
			_IsPlaying = true;
			_CurrentFrame = 0;
			//アニメーションのグローバルタイムをリセット
			_AnimController->ResetTime();
			_CurrentAnimationSetNo = animationSetIndex;
			//補間開始の印。
			_IsInterpolate = true;
			this->_InterpolateTime = 0.0f;
			_InterpolateEndTime = _InterpolateTime;
			_CurrentTrackNo = (_CurrentTrackNo + 1) % _NumMaxTracks;
			//トラックにアニメーションセット
			_AnimController->SetTrackAnimationSet(_CurrentTrackNo, _AnimationSets[(_NumAnimSet - 1) - _CurrentAnimationSetNo]);
			_AnimController->SetTrackEnable(_CurrentTrackNo, TRUE);
			_AnimController->SetTrackPosition(_CurrentTrackNo, 0.0f);
			SetAnimeSpeed(1.0f);
			_AnimController->AdvanceTime(0, NULL);
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
		_CurrentFrame++;
		if (_AnimController == nullptr)
			return;
		//現在のトラックのアニメーションセット取得
		LPD3DXANIMATIONSET aniset;
		_AnimController->GetTrackAnimationSet(_CurrentTrackNo, &aniset);
		//そのアニメーションを再生しきるまでの時間
		double maxtime = aniset->GetPeriod() / (double)_PlaySpeed;
		//現在のアニメーションの時間を取得
		_NowTime = aniset->GetPeriodicPosition(_AnimController->GetTime());
		double delta = Time::DeltaTime();
		//割合
		_TimeRatio = min(1.0f, (_NowTime + delta) / maxtime);

		//アニメーションの時間加算
		//_AnimController->AdvanceTime(delta, NULL);
		_AnimController->AdvanceTime(1.0f/60.0f, NULL);

		if (_IsInterpolate) {
			//補間中。
			_InterpolateTime += delta;
			float weight = 0.0f;
			if (_InterpolateTime > _InterpolateEndTime) {
				//補間終了。
				_IsInterpolate = false;
				weight = 1.0f;
				_AnimController->SetTrackWeight(_CurrentTrackNo, weight);
				//現在のトラック以外を無効にする。
				for (int i = 0; i < _NumMaxTracks; i++) {
					if (i != _CurrentTrackNo) {

						_AnimController->SetTrackEnable(i, FALSE);
					}
				}
			}
			else {
				weight = _InterpolateTime / _InterpolateEndTime;
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

		//再生時間を超えた
		if (maxtime <= _NowTime + delta)
		{
			_CurrentFrame = 0;
			_LoopCount++;
			//無限ループではない
			//カウントが指定した数以上になった
			if (_LoopNum != -1 &&
				_LoopCount >= _LoopNum)
			{
				//アニメーション終了
				_IsPlaying = false;
				//最後の方で止めておく
				_AnimController->SetTrackPosition(_CurrentTrackNo, maxtime - 0.001f);
				_AnimController->AdvanceTime(0,NULL);
			}
		}

	}
}