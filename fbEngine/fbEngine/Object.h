#pragma once
//オブジェクトの基底クラス
class Object : Noncopyable
{
public:
	Object()
	{
		_Name = "Empty";
	}
	Object(const char* name)
	{
		_Name = new char[strlen(name) + 1];
		//文字列コピー
		strcpy(_Name, name);
	}
	virtual ~Object()
	{
		SAFE_DELETE(_Name);
	};

	//インスタンスが作成された時に実行される
	//純粋仮想関数ではないので絶対オーバーライドしなくてもいい
	virtual void Awake(){};

	//シーンの初期化中に実行される。
	//初期化を行う純粋仮想関数
	virtual void Start(){};

	//更新を行う純粋仮想関数
	virtual void Update(){};

	//更新の後に呼び出される関数
	//純粋仮想関数ではないので絶対オーバーライドしなくてもいい
	virtual void LateUpdate(){};

	//描画の前に呼び出されます
	virtual void PreRender(){};
	//描画を行う
	virtual void Render(){};
	//ポストエフェクト描画
	virtual void PostRender(){};
	//イメージ(2D)描画
	virtual void ImageRender(){};

	const char* GetName() const
	{
		return _Name;
	}
protected:
	//オブジェクトの名前
	char* _Name;
};