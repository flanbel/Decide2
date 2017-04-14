/*!
 * @brief	アニメーションクラス。
 */
#include <memory>
#include "Component.h"
#include "TextObject.h"

/*!
 * @brief	アニメーションクラス。
 */
class Animation:public Component{
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
	}
	
	void Initialize(ID3DXAnimationController* anim);
	void Awake();
	void Update();

	void PlayAnimation(const int& animationSetIndex);
	/*!
	*@brief	アニメーションの再生。アニメーションの補完が行われます。
	*@param[in]		animationSetIndex	再生したいアニメーションのインデックス。
	*@param[in]		_InterpolateTimer		補間時間。
	//第三引数　ループ数
	*/
	void PlayAnimation(int animationSetIndex, float _InterpolateTimer,int loopnum = -1);
	/*!
	*@brief	アニメーションセットの取得。
	*/
	int GetNumAnimationSet() const
	{
		return _NumAnimSet;
	}
	//現在再生中のアニメーション番号取得
	int GetPlayAnimNo() const
	{
		return _CurrentAnimationSetNo;
	}
	//アニメーションが終了しているかどうか
	bool GetPlaying()
	{
		return _IsPlaying;
	}

	double GetLocalAnimationTime()
	{
		return _LocalAnimationTime;
	}

	int NowFrame()
	{
		return _CurrentFrame;
	}
	//割合取得
	double GetTimeRatio()
	{
		return _TimeRatio;
	}

	void SetAnimeSpeed(float sp)
	{
		_PlaySpeed = sp;
		//_AnimController->SetTrackSpeed(_CurrentTrackNo, sp);
	}
	/*!
	*@brief	アニメーション終了時間設定
	*@param[in]		idx	アニメーションセットの番号。
	*@param[in]		endtime		終了時間。
	*/
	void SetAnimationEndTime(int idx,double endtime)
	{
		if (idx == 4) {
			OutputDebugString(_T("プレイヤー走る\n"));
		}
		_EndTime[idx] = endtime;
	}
	//アニメーションのローカルタイム設定
	void SetLocalAnimationTime(UINT track,double t)
	{
		_LocalAnimationTime = t;
		//時間設定。
		_AnimController->SetTrackPosition(track, _LocalAnimationTime);
		//ポジションを更新するために呼ばなければいけない
		_AnimController->AdvanceTime(0.0, NULL);
	}
private:
	ID3DXAnimationController*				_AnimController;		//!<アニメーションコントローラ。
	int										_NumAnimSet;				//!<アニメーションセットの数。
	std::unique_ptr<ID3DXAnimationSet*[]>	_AnimationSets;			//!<アニメーションセットの配列。
	std::unique_ptr<float[]>				_BlendRateTable;			//!<ブレンディングレートのテーブル。
	int										_CurrentAnimationSetNo;	//!<現在再生中のアニメーショントラックの番号。
	int										_CurrentTrackNo;			//!<現在のトラックの番号。
	int										_NumMaxTracks;			//!<アニメーショントラックの最大数。
	bool									_IsBlending;				//!<アニメーションブレンディング中？
	bool									_IsInterpolate;			//!<補間中？
	float									_InterpolateEndTime;		//!<補間終了時間。
	float									_InterpolateTimer;		//!<補間時間。

	std::unique_ptr<double[]> _EndTime;	//各アニメーションの終了時間を格納した配列
	double _TimeRatio;					//正規化された時間の割合。
	double _LocalAnimationTime;			//ローカルなアニメーションの経過時間
	int _CurrentFrame;					//アニメーションが再生されて現在何フレーム目か。
	float _PlaySpeed;					//再生速度
	int _LoopNum;						//アニメーションをループさせる数。
	int _LoopCount;						//ループ数をカウントする。
	bool _IsPlaying;					//アニメーション再生中であることを示す。
};