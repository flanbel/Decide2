/*!
 * @brief	アニメーションクラス。
 */
#pragma once
#include "_Object\_Component\Component.h"

/*!
 * @brief	アニメーションクラス。
 */
class Animation : public Component{
public:
	/*!
	 * @brief	コンストラクタ
	 */
	Animation(GameObject* g, Transform* t) :Component(g,t,typeid(this).name())
	{
		
	}
	/*!
	 * @brief	デストラクタ。
	 */
	~Animation()
	{
		//ユニークポインタ破棄
		_AnimationSets.release();
		_BlendRateTable.release();
		_EndTime.release();
	}
	
	void Initialize(ID3DXAnimationController* anim);
	void Awake();
	void Update();
	//アニメーション再生(補完なし)
	void PlayAnimation(const UINT& animationSetIndex);
	//アニメーション再生(補完あり)
	//第1引数　再生したいアニメーションのインデックス
	//第2引数　補間時間
	//第3引数　ループ数
	void PlayAnimation(const UINT& animationSetIndex, const float& interpolateTime, const int& loopnum = -1);
	//アニメーション再生(補完あり)
	//第1引数　再生したいアニメーションのインデックス
	//第2引数　補間時間
	//第3引数　ループ数
	void PlayAnimation(const UINT& animationSetIndex, const float& interpolateTime, const float& transitionTime, const int& loopnum = -1);
	//現在再生中のアニメーション番号取得
	const UINT& GetPlayAnimNo() const
	{
		return _CurrentAnimationSetNo;
	}
	//アニメーションが終了しているかどうか
	const bool& GetPlaying()
	{
		return _IsPlaying;
	}

	const double& GetLocalAnimationTime()
	{
		return _LocalAnimationTime;
	}

	const double& NowFrame()
	{
		return _CurrentFrame;
	}
	//割合取得
	const double& GetTimeRatio()
	{
		return _TimeRatio;
	}

	void SetAnimeSpeed(float sp)
	{
		_PlaySpeed = sp;
	}
	/*!
	*@brief	アニメーション終了時間設定
	*@param[in]		idx	アニメーションセットの番号。
	*@param[in]		endtime		終了時間。
	*/
	void SetAnimationEndTime(const UINT& idx, const double& endtime)
	{
		//範囲内か？
		if (idx < _NumAnimSet)
			_EndTime[idx] = endtime;
	}
	const double& GetAnimationEndTime(const UINT& idx)
	{
		//範囲内か？
		if (idx < _NumAnimSet)
			return _EndTime[idx];
	}
	//アニメーションのローカルタイム設定
	void SetLocalAnimationTime(const UINT& track, const double& t)
	{
		_LocalAnimationTime = t;
		//時間設定。
		_AnimController->SetTrackPosition(track, _LocalAnimationTime);
		//ポジションを更新するために呼ばなければいけない
		_AnimController->AdvanceTime(0.0, NULL);
	}
private:
	ID3DXAnimationController*				_AnimController;		//!<アニメーションコントローラ。
	UINT									_NumAnimSet;				//!<アニメーションセットの数。
	std::unique_ptr<ID3DXAnimationSet*[]>	_AnimationSets;			//!<アニメーションセットの配列。
	std::unique_ptr<float[]>				_BlendRateTable;			//!<ブレンディングレートのテーブル。
	UINT									_CurrentAnimationSetNo;	//!<現在再生中のアニメーショントラックの番号。
	UINT									_CurrentTrackNo;			//!<現在のトラックの番号。
	UINT									_NumMaxTracks;			//!<アニメーショントラックの最大数。
	bool									_IsBlending;				//!<アニメーションブレンディング中？
	bool									_IsInterpolate;			//!<補間中？
	float									_InterpolateEndTime;		//!<補間終了時間。
	float									_InterpolateTimer;		//!<補間時間。

	std::unique_ptr<double[]> _EndTime;	//各アニメーションの終了時間を格納した配列
	double _TimeRatio;					//正規化された時間の割合。
	double _LocalAnimationTime;			//ローカルなアニメーションの経過時間
	double _CurrentFrame;					//アニメーションが再生されて現在何フレーム目か。
	float _PlaySpeed;					//再生速度
	int _LoopNum;						//アニメーションをループさせる数。
	int _LoopCount;						//ループ数をカウントする。
	bool _IsPlaying;					//アニメーション再生中であることを示す。
};