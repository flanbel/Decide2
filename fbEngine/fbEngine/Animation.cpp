#include "Animation.h"

void Animation::Initialize(ID3DXAnimationController* anim)
{
	animController = anim;
	numAnimSet = animController->GetMaxNumAnimationSets();
	numMaxTracks = animController->GetMaxNumTracks();
	blendRateTable.reset(new float[numMaxTracks]);
	animationSets.reset(new ID3DXAnimationSet*[numAnimSet]);
	for (int i = 0; i < numMaxTracks; i++){
		blendRateTable[i] = 1.0f;
	}
	//アニメーションセットを初期化。
	for (int i = 0; i < numAnimSet; i++) {
		animController->GetAnimationSet(i, &animationSets[i]);
		animController->SetTrackPosition(i, 0.0f);
		animController->AdvanceTime(0, NULL);
	}
}

void Animation::Awake()
{
	animController = nullptr;
	numAnimSet = 0;
	isBlending = false;
	isInterpolate = false;
	numMaxTracks = 0;
	interpolateTime = 0.0f;
	interpolateEndTime = 0.0f;
	currentTrackNo = 0;
}

void Animation::PlayAnimation(const int& animationSetIndex)
{
	if (animationSetIndex < numAnimSet) {
		if (animController) {
			//再生開始
			isPlaying = true;
			//アニメーションのグローバルタイムをリセット
			animController->ResetTime();
			currentAnimationSetNo = (numAnimSet - 1) - animationSetIndex;
			currentTrackNo = 0;
			//0番目以外のトラックは無効にする。
			for (int i = 1; i < numMaxTracks; i++) {
				animController->SetTrackEnable(i, FALSE);
			}
			animController->SetTrackWeight(0, 1.0f);
			animController->SetTrackAnimationSet(currentTrackNo, animationSets[currentAnimationSetNo]);
			animController->SetTrackEnable(0, TRUE);
			animController->SetTrackPosition(0, 0.0f);
			animController->AdvanceTime(0, NULL);
			SetAnimeSpeed(1.0f);
		}
	}
	else {
		//TK_LOG("warning!!! animationSetIndex is over range!!!!!");
	}
}

void Animation::PlayAnimation(int animationSetIndex, float interpolateTime, int lnum)
{
	if (animationSetIndex < numAnimSet) {
		if (animController) {
			//ループ数設定
			loopNum = lnum;
			loopCount = 0;
			//再生開始
			isPlaying = true;
			//アニメーションのグローバルタイムをリセット
			animController->ResetTime();
			//補間開始の印。
			isInterpolate = true;
			this->interpolateTime = 0.0f;
			interpolateEndTime = interpolateTime;
			currentTrackNo = (currentTrackNo + 1) % numMaxTracks;
			//トラックにアニメーションセット
			animController->SetTrackAnimationSet(currentTrackNo, animationSets[(numAnimSet - 1) - animationSetIndex]);
			animController->SetTrackEnable(currentTrackNo, TRUE);
			animController->SetTrackPosition(currentTrackNo, 0.0f);
			SetAnimeSpeed(1.0f);
			animController->AdvanceTime(0, NULL);
		}
	}
	else {
		//TK_LOG("warning!!! animationSetIndex is over range!!!!!");
	}
}

void Animation::Update()
{
	//指定されたループ数ないかどうか
	if (isPlaying)
	{

		if (animController == nullptr)
			return;
		//現在のトラックのアニメーションセット取得
		LPD3DXANIMATIONSET aniset;
		animController->GetTrackAnimationSet(currentTrackNo, &aniset);
		//そのアニメーションを再生しきるまでの時間
		double maxtime = aniset->GetPeriod();
		//現在のアニメーションの時間を取得
		nowTime = aniset->GetPeriodicPosition(animController->GetTime());

		//アニメーションの時間加算
		animController->AdvanceTime(Time::DeltaTime(), NULL);

		if (isInterpolate) {
			//補間中。
			interpolateTime += Time::DeltaTime();
			float weight = 0.0f;
			if (interpolateTime > interpolateEndTime) {
				//補間終了。
				isInterpolate = false;
				weight = 1.0f;
				animController->SetTrackWeight(currentTrackNo, weight);
				//現在のトラック以外を無効にする。
				for (int i = 0; i < numMaxTracks; i++) {
					if (i != currentTrackNo) {
						
						animController->SetTrackEnable(i, FALSE);
					}
				}
			}
			else {
				weight = interpolateTime / interpolateEndTime;
				float invWeight = 1.0f - weight;
				//ウェイトを設定していく。
				for (int i = 0; i < numMaxTracks; i++) {
					if (i != currentTrackNo) {
						animController->SetTrackWeight(i, blendRateTable[i] * invWeight);
					}
					else {
						animController->SetTrackWeight(i, weight);
					}
				}
			}
		}

		//再生時間を超えた
		if((maxtime / playSpeed) < nowTime + Time::DeltaTime())
		{
			loopCount++;
			//無限ループではない
			//カウントが指定した数以上になった
			if (loopNum != -1 &&
				loopCount >= loopNum)
			{
				//アニメーション終了
				isPlaying = false;
				float delta = maxtime - nowTime - 0.0001f;
				animController->SetTrackSpeed(currentTrackNo, 0.0f);
				animController->AdvanceTime(delta, NULL);
			}
		}

	}
}