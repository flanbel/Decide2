#pragma once

class GameObject;
class Camera;
class Light;
class ShadowCamera;

//オブジェクトを管理するクラス
class GameObjectManager : Noncopyable
{
public:
	GameObjectManager()
	{
		
	}

	//マネージャーにオブジェクトを追加する
	//戻り値：GameObject* 追加に成功したオブジェクトのポインタ
	//第一引数：GameObject* アドレス
	//第二引数：int 優先度
	static GameObject* Add(GameObject* pAdd,int priority);

	template<class T>
	static T* AddNew(char* name, int priority)
	{
		//優先度が超えてる
		if(priority >= System::MAX_PRIORITY)
		{
			priority = System::MAX_PRIORITY - 1;
		}
		if (_GameObjects.size() <= 0)
		{
			_GameObjects.resize(System::MAX_PRIORITY);
		}
		T* obj = new T(name);
		_GameObjects.at(priority).push_back(obj);
		obj->Awake();

		return obj;
	}

	//Updateの前に一度だけ呼び出される
	static void StartObject();

	//毎フレーム呼び出される。
	static void UpdateObject();

	//毎フレームUpdateよりも後に呼び出される。
	static void LateUpdateObject();
	//描画の前に呼び出される
	static void PreRenderObject();

	//描画の際に呼び出される
	static void RenderObject();
	//描画の際に呼び出される
	static void PostRenderObject();
	//描画の際に呼び出される
	static void ImageRenderObject();
	
	//破棄リストに追加
	static void AddRemoveList(GameObject* obj);
	static void AddRemoveList(char* name);

	//オブジェクト検索関数
	//戻り値：GameObject* ヒットしたオブジェクトのアドレス、あるいはnullポインタ
	//第一引数：char* 検索したいオブジェクトの名前
	//一番最初にヒットした者しか返さないので注意されたし。
	static GameObject* FindObject(char* name);

	//未実装
	static bool FindObjects(char* name,GameObject** objArray);

	//確保しているオブジェクトを解放
	static void Release();

	//使用するカメラ
	static Camera* mainCamera;
	//使用するライト
	static Light* mainLight;
	//影を作るためのカメラ
	static ShadowCamera* mainShadowCamera;

	struct RemoveObj
	{
		int prio;
		list<GameObject*>::iterator it;
		RemoveObj(list<GameObject*>::iterator i, int p)
		{
			it = i;
			prio = p;
		}
	};
private:
	//ゲームオブジェクトを管理
	static vector<list<GameObject*>> _GameObjects;
	//削除リスト
	static list<RemoveObj> _RemoveList;
	//削除リストのオブジェクトを削除する。
	static void _RemoveObject();
};