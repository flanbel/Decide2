#include "SceneManager.h"
#include "Scene.h"

SceneManager*  SceneManager::instance;

#include "ImageObject.h"
#include "Sprite.h"

SceneManager::SceneManager()
{
	offScreen = new ImageObject("OffScreen");
	offScreen->Awake();
	sprite = offScreen->GetComponent<Sprite>();
	sprite->Start();
	//レンダーターゲット作成
	rt = new RenderTarget();
	rt->Create(g_WindowSize * 2);
	rt->texture->size = rt->texture->size / 2;

	//レンダーターゲットのテクスチャを取得
	offScreen->SetTexture(rt->texture);
}

SceneManager::~SceneManager()
{
}

void SceneManager::Add(Scene* pAdd)
{
	scenes.push_back(pAdd);
}
#include "FPS.h"
void SceneManager::StartScene()
{
#ifdef _DEBUG
	GameObjectManager::AddNew<FPS>("fps", MAX_PRIORITY);
#endif // DEBUG
	scenes[nowScene]->Start();
	GameObjectManager::StartObject();
}

void SceneManager::UpdateScene()
{
	scenes[nowScene]->Update();
	GameObjectManager::UpdateObject();
	PhysicsWorld::Instance()->Update();
	GameObjectManager::LateUpdateObject();
}

void SceneManager::DrawScene()
{
	//0番目に設定(深度書き込み用バッファ)
	INSTANCE(RenderTargetManager)->ReSetRenderTarget(0, INSTANCE(RenderTargetManager)->GetRenderTarget(0));
	//影とかのやつ
	GameObjectManager::PreRenderObject();

	//0番目に設定(オフスクリーンレンダリング用)
	INSTANCE(RenderTargetManager)->ReSetRenderTarget(0, rt);
	GameObjectManager::RenderObject();

	//レンダーターゲットを元に戻す
	INSTANCE(RenderTargetManager)->BeforeRenderTarget();
	GameObjectManager::PostRenderObject();
	//オフスクリーンのやつ描画(ブルームが完成するまで)
	sprite->ImageRender();
	//2Dとか？
	GameObjectManager::ImageRenderObject();
}

Scene* SceneManager::ChangeScene(int key)
{
	//シーンの添え字切り替え
	nowScene = key;
	//オブジェクトリリース
	GameObjectManager::Release();
	//初期化する
	SceneManager::StartScene();
	return scenes[nowScene];
}

Scene* SceneManager::ChangeScene(char * Scenename)
{
	//クラス名
	char* classname = new char[128];
	strcpy_s(classname, Length("class "), "class ");
	strcat_s(classname, Length(Scenename) + Length(classname), Scenename);
	int idx = 0;
	for each (Scene* s in scenes)
	{
		//名前の一致
		if (strcmp(classname, typeid(*s).name()) == 0)
		{
			nowScene = idx;
			//オブジェクトリリース
			GameObjectManager::Release();
			//初期化する
			SceneManager::StartScene();
			return scenes[nowScene];
		}
		idx++;
	}
	return nullptr;
}

TEXTURE* SceneManager::GetOffScreenTexture()
{
	return rt->texture;
}