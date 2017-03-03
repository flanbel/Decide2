#include "SceneManager.h"
#include "Scene.h"

SceneManager*  SceneManager::instance;

#include "ImageObject.h"
#include "Sprite.h"


SceneManager::SceneManager()
{
	_OffScreen = new ImageObject("OffScreen");
	_OffScreen->Awake();
	_Sprite = _OffScreen->GetComponent<Sprite>();
	_Sprite->Start();
	//レンダーターゲット作成
	_RT = new RenderTarget();
	//大きさ二倍で作って高画質に
	_RT->Create(g_WindowSize * 2);
	//掛ける倍率は0.5ｆ
	_RT->texture->size = _RT->texture->size / 2;

	//レンダーターゲットのテクスチャを取得
	_Sprite->SetTexture(_RT->texture);
	_Sprite->SetPivot(Vector2(0.0f, 0.0f));
	//ブルームの準備
	_Bloom.Create();
}

SceneManager::~SceneManager()
{
}

void SceneManager::Add(Scene* pAdd)
{
	_Scenes.push_back(pAdd);
}
#include "FPS.h"
void SceneManager::StartScene()
{
#ifdef _DEBUG
	GameObjectManager::AddNew<FPS>("fps", MAX_PRIORITY);
#endif // DEBUG
	_Scenes[_NowScene]->Start();
	GameObjectManager::StartObject();
}

void SceneManager::UpdateScene()
{
	_Scenes[_NowScene]->Update();
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
	INSTANCE(RenderTargetManager)->ReSetRenderTarget(0, _RT);
	GameObjectManager::RenderObject();

	//レンダーターゲットを元に戻す
	INSTANCE(RenderTargetManager)->BeforeRenderTarget();
	
	GameObjectManager::PostRenderObject();
	//オフスクリーンのやつ描画
	_Sprite->ImageRender();
	_Bloom.Render();
	
	//2Dとか？
	GameObjectManager::ImageRenderObject();
}

Scene* SceneManager::ChangeScene(int key)
{
	//シーンの添え字切り替え
	_NowScene = key;
	//オブジェクトリリース
	GameObjectManager::Release();
	//初期化する
	SceneManager::StartScene();
	return _Scenes[_NowScene];
}

Scene* SceneManager::ChangeScene(char * Scenename)
{
	//クラス名
	char* classname = new char[128];
	strcpy_s(classname, Length("class "), "class ");
	strcat_s(classname, Length(Scenename) + Length(classname), Scenename);
	int idx = 0;
	for each (Scene* s in _Scenes)
	{
		//名前の一致
		if (strcmp(classname, typeid(*s).name()) == 0)
		{
			_NowScene = idx;
			//オブジェクトリリース
			GameObjectManager::Release();
			//初期化する
			SceneManager::StartScene();
			return _Scenes[_NowScene];
		}
		idx++;
	}
	return nullptr;
}

TEXTURE* SceneManager::GetOffScreenTexture()
{
	return _RT->texture;
}