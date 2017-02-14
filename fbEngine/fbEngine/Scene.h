#pragma once

//シーンの基底クラス
class Scene
{
public:
	Scene()
	{
	
	}
	virtual void Start() {};
	virtual void Update() {};
	virtual void LateUpdate() {};
	virtual void Render() {};

	//オブジェクトマネージャーの中身を全てリリース
	void GameObjectRelease();

	/*GameObjectManager& GetManager()
	{
		return gameObjectManager;
	}*/
protected:
	//ゲームオブジェクトを管理する
	//GameObjectManager gameObjectManager;
};
