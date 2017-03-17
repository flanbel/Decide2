/*!
 * @brief	アニメーションクラス。
 */
#include <memory>
#include "Component.h"

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
	*@param[in]		_InterpolateTime		補間時間。
	//第三引数　ループ数
	*/
	void PlayAnimation(int animationSetIndex, float _InterpolateTime,int loopnum = -1);
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

	double GetNowTime()
	{
		return _NowTime;
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
		_AnimController->SetTrackSpeed(_CurrentTrackNo, sp);
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
	float									_InterpolateTime;		//!<補間時間。

	double _TimeRatio;	//正規化された時間の割合。
	double _NowTime;	//現在のアニメーションの時間
	int _CurrentFrame;		//現在のフレーム数
	float _PlaySpeed;//再生速度
	int _LoopNum;//指定したループ数
	int _LoopCount;//ループのカウント
	bool _IsPlaying;//アニメーション再生中
};