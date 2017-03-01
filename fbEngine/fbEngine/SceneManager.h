#pragma once

#include "Bloom.h"

class Scene;
class Sprite;
class ImageObject;
struct TEXTURE;
struct RenderTarget;

//シーンを管理するクラス
class SceneManager : Noncopyable
{
public:
	SceneManager();
	~SceneManager();
	//シーンの追加
	void Add(Scene* pAdd);

	template<class T>
	void Add()
	{
		scenes.push_back(new T);
	}
	//シーンの初期化を行う
	void StartScene();
	//シーンの更新を行う
	void UpdateScene();
	//シーンの描画を行う
	void DrawScene();
	//シーンの切り替え
	Scene* ChangeScene(int key);
	Scene* ChangeScene(char* Scenename);
	template<class T>
	Scene* ChangeScene()
	{
		const char* name = typeid(T).name();
		int idx = 0;
		for each (Scene* s in scenes)
		{
			//名前の一致
			if (name == typeid(*s).name())
			{
				nowScene = idx;
				return scenes[nowScene];
			}
			idx++;
		}
		return nullptr;
	}
	static SceneManager* Instance()
	{
		if (instance == nullptr)
		{
			instance = new SceneManager();
		}
		return instance;
	}
	TEXTURE* GetOffScreenTexture();
private:
	int nowScene;	//現在のシーンの添え字
	vector<Scene*> scenes;
	ImageObject* offScreen;
	Sprite* sprite;
	RenderTarget* rt;
	static SceneManager* instance;

	Bloom bloom;
};