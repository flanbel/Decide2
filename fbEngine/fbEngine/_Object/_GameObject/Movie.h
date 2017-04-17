#pragma once
#include "GameObject.h"
//動画再生クラス
class Movie:public GameObject
{
public:
	Movie(const char* name) :GameObject(name) {}

	void Awake()override;
	void Start()override;
	void Update()override;
private:
	//GraphBuilderインターフェース
	IGraphBuilder* _GraphBuilder;
	//レンダリングを担当するVMR9フィルタ
	IBaseFilter* _BaseFilterVMR9;
	//メディアコントローラー
	IMediaControl* _MediaControl;
	//メディアイベント
	IMediaEvent* _MediaEvent;
};