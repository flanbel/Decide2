#pragma once
#include "GameObject.h"

class Light;
class Camera;
class ShadowCamera;
namespace
{
	//削除するときに使う
	struct RemoveObj
	{
	public:
		RemoveObj(list<GameObject*>::iterator i, int p)
		{
			iterator = i;
			priority = p;
		}
		
		//	//sortに使う
		//bool operator<(const RemoveObj& left) const
		//{
		//	//アドレスが異なるなら交換
		//	if (left.addres != this->addres)
		//	{
		//		return true;
		//	}
		//	//並び替えしない
		//	return false;
		//}
		//
		////比較演算子 uniqueに使う
		//bool operator==(const RemoveObj& left)
		//{
		//	//アドレス比較
		//	if (left.addres == this->addres)
		//	{
		//		return true;
		//	}
		//	return false;
		//}
		
		int priority;								//優先度
		list<GameObject*>::iterator iterator;		//イテレータ
	};
}

//オブジェクトを管理するクラス
class GameObjectManager
{
public:
	GameObjectManager()
	{
		//ベクターリサイズ
		if (_GameObjects.size() <= 0)
		{
			_GameObjects.resize(System::MAX_PRIORITY + 1);
		}
	}

	//マネージャーにオブジェクトを追加する
	//戻り値：GameObject* 追加に成功したオブジェクトのポインタ
	//第一引数：GameObject* アドレス
	//第二引数：int 優先度
	GameObject* Add(GameObject* pAdd,int priority);

	template<class T>
	T* AddNew(char* name, UINT priority)
	{
		//優先度が超えてる
		if(priority > System::MAX_PRIORITY)
		{
			priority = System::MAX_PRIORITY - 1;
		}
		T* obj = new T(name);
		_GameObjects.at(priority).push_back(obj);
		obj->Awake();

		return obj;
	}

	//Updateの前に一度だけ呼び出される
	void StartObject();

	//毎フレーム呼び出される。
	void UpdateObject();

	//毎フレームUpdateよりも後に呼び出される。
	void LateUpdateObject();
	//描画の前に呼び出される
	void PreRenderObject();

	//描画の際に呼び出される
	void RenderObject();
	//描画の際に呼び出される
	void PostRenderObject();
	//描画の際に呼び出される
	void ImageRenderObject();
	
	//破棄リストに追加
	void AddRemoveList(GameObject* obj);
	void AddRemoveList(char* name);

	//オブジェクト検索関数
	//戻り値：GameObject* ヒットしたオブジェクトのアドレス、あるいはnullポインタ
	//第一引数：char* 検索したいオブジェクトの名前
	//一番最初にヒットした者しか返さないので注意されたし。
	GameObject* FindObject(char* name);

	//未実装
	bool FindObjects(char* name,GameObject** objArray);

	//確保しているオブジェクトを解放
	void Release();

	static GameObjectManager* Instance()
	{
		if (_Instance == nullptr)
		{
			_Instance = new GameObjectManager();
		}
		return _Instance;
	}
public:
	//使用するカメラ
	Camera* mainCamera;
	//使用するライト
	Light* mainLight;
	//影を作るためのカメラ
	ShadowCamera* mainShadowCamera;
private:
	//リストに重複がないかチェックする。
	bool _CheckUniqueRemoveList(GameObject* obj);
	//削除リストのオブジェクトを削除する。
	void _RemoveObject();
private:
	//ゲームオブジェクトを管理するリスト
	vector<list<GameObject*>> _GameObjects;
	//削除リスト
	list<RemoveObj> _RemoveList;
	static GameObjectManager* _Instance;
};