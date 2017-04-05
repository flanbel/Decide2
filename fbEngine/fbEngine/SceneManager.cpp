#include "SceneManager.h"
#include "Scene.h"

SceneManager*  SceneManager::_Instance;

#include "ImageObject.h"


SceneManager::SceneManager()
{
	_OffScreen = new ImageObject("OffScreen");
	_OffScreen->Awake();
	_Sprite = _OffScreen->GetComponent<Sprite>();
	_Sprite->Start();
	//レンダーターゲット作成
	_RT = new RenderTarget();
	//大きさ二倍で作って高画質に
	//浮動小数点バッファで作成
	_RT->Create(g_WindowSize * 2, D3DFMT_A16B16G16R16F);
	//掛ける倍率は0.5ｆ
	_RT->texture->Size = _RT->texture->Size / 2;

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
//#ifdef _DEBUG
	FPS* fps = GameObjectManager::AddNew<FPS>("fps", System::MAX_PRIORITY);
	fps->transform->SetLocalPosition(Vector3(0, 30, 0));
//#endif // DEBUG
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
	//0番目に設定(影の深度書き込み用バッファ)
	INSTANCE(RenderTargetManager)->ReSetRT(0, INSTANCE(RenderTargetManager)->GetRTFromList(RTIdxE::SHADOWDEPTH));
	//事前描画(影とか深度とか輝度とか)
	GameObjectManager::PreRenderObject();

	//0番目に設定(オフスクリーンレンダリング用)
	INSTANCE(RenderTargetManager)->ReSetRT(0, _RT);
	GameObjectManager::RenderObject();
	//レンダーターゲットを元に戻す
	INSTANCE(RenderTargetManager)->BeforeRT();
	
	//オフスクリーンのやつ描画
	_Sprite->ImageRender();
	_Bloom.Render();
	GameObjectManager::PostRenderObject();
	
	//2Dとか？
	GameObjectManager::ImageRenderObject();

	//シーンのフェードのやつ
	//最前面に来るように最後に描画
	_Scenes[_NowScene]->Fade();
}

Scene* SceneManager::ChangeScene(int key)
{
	//フェードが明ける
	Scene::SetFade(false);
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
	strcpy_s(classname, strlen("class ")+1, "class ");
	strcat_s(classname, strlen(Scenename) + strlen(classname)+1, Scenename);
	int idx = 0;
	for each (Scene* s in _Scenes)
	{
		//名前の一致
		if (strcmp(classname, typeid(*s).name()) == 0)
		{
			//シーン切り替え
			return ChangeScene(idx);
		}
		idx++;
	}
	return nullptr;
}

TEXTURE* SceneManager::GetOffScreenTexture()
{
	return _RT->texture;
}