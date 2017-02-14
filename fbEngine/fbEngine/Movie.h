#pragma once
#include "GameObject.h"
//動画再生クラス
class Movie:public GameObject
{
public:
	Movie(char* name) :GameObject(name) {}

	void Awake()override;
	void Start()override;
	void Update()override;
private:
	//GraphBuilderインターフェース
	IGraphBuilder* graphBuilder;
	//レンダリングを担当するVMR9フィルタ
	IBaseFilter* baseFilterVMR9;
	//メディアコントローラー
	IMediaControl* mediaControl;
	//メディアイベント
	IMediaEvent* mediaEvent;
};