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
	Animation(GameObject* g, Transform* t) :Component(g,t)
	{
		name = (char*)typeid(*this).name();
	}
	/*!
	 * @brief	デストラクタ。
	 */
	~Animation()
	{
		//ユニークポインタ破棄
		animationSets.release();
		blendRateTable.release();
	}
	
	void Initialize(ID3DXAnimationController* anim);
	void Awake();
	void Update();

	void PlayAnimation(const int& animationSetIndex);
	/*!
	*@brief	アニメーションの再生。アニメーションの補完が行われます。
	*@param[in]		animationSetIndex	再生したいアニメーションのインデックス。
	*@param[in]		interpolateTime		補間時間。
	//第三引数　ループ数
	*/
	void PlayAnimation(int animationSetIndex, float interpolateTime,int loopnum = -1);
	/*!
	*@brief	アニメーションセットの取得。
	*/
	int GetNumAnimationSet() const
	{
		return numAnimSet;
	}
	//アニメーションが終了しているかどうか
	bool GetPlaying()
	{
		return isPlaying;
	}

	double GetNowTime()
	{
		return nowTime;
	}

	void SetAnimeSpeed(float sp)
	{
		playSpeed = sp;
		animController->SetTrackSpeed(currentTrackNo, sp);
	}
private:
	ID3DXAnimationController*				animController;		//!<アニメーションコントローラ。
	int										numAnimSet;				//!<アニメーションセットの数。
	std::unique_ptr<ID3DXAnimationSet*[]>	animationSets;			//!<アニメーションセットの配列。
	std::unique_ptr<float[]>				blendRateTable;			//!<ブレンディングレートのテーブル。
	int										currentAnimationSetNo;	//!<現在再生中のアニメーショントラックの番号。
	int										currentTrackNo;			//!<現在のトラックの番号。
	int										numMaxTracks;			//!<アニメーショントラックの最大数。
	bool									isBlending;				//!<アニメーションブレンディング中？
	bool									isInterpolate;			//!<補間中？
	float									interpolateEndTime;		//!<補間終了時間。
	float									interpolateTime;		//!<補間時間。

	double nowTime;
	float playSpeed;//再生速度
	int loopNum;//指定したループ数
	int loopCount;//ループのカウント
	bool isPlaying;//アニメーション再生中
};