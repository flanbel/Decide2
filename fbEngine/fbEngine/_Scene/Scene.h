#pragma once

class Vertex;
class Effect;

//シーンの基底クラス
class Scene
{
public:
	Scene();
	~Scene();
	//初期化関数
	//シーンが切り替えられた後に呼ばれる
	virtual void Start() {};
	//更新関数
	//切り替えられたら毎フレーム呼び出される処理(いるのか？)
	virtual void Update() {};

	//フェード
	//フェード中はtrueが帰る。
	void Fade();
	//第一引数はフェードインかフェードアウトか指定
	//第二引数はフェードにかかる時間(秒)
	static void SetFade(bool inout,float fadetime = 2.0f);
private:
	//全てのシーンで共有したいのでstatic
	//頂点
	static Vertex *_Vertex;
	//シェーダ
	static Effect *_Effect;
	//フェードのα
	static float _FadeAlpha;
	//足すα値
	static float _AddAlpha;
protected:
	//フェード中
	static bool _IsFade;
	//シーン切り替えフラグ
	bool _ChangeScene;
	//シーン切り替えの処理を書く
	//virtual void _ChangeScene() = 0;
};
