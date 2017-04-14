#pragma once
#include "Object.h"
#include "ComponentManager.h"

class Component;

//ゲームオブジェクトの基礎クラス
class GameObject : public Object
{
public:
	GameObject();
	GameObject(const char* name);
	virtual ~GameObject();

	/*virtual void Awake() {};
	virtual void Start() {};
	virtual void Update() {};
	virtual void LateUpdate() {};
	virtual void PreRender() {};
	virtual void Render() {};*/

	//コンポーネント追加
	template<class T>
	T* AddComponent()
	{
		return _Components.AddComponent<T>(this,transform);
	}
	//コンポーネント追加
	void AddComponent(Component* c)
	{
		return _Components.AddComponent(c);
	}
	//名前からコンポーネント取得
	Component* GetComponent(const char* name)
	{
		return _Components.GetComponent(name);
	}
	//コンポーネント取得
	template <class T>
	T* GetComponent()
	{
		return _Components.GetComponent<T>();
	}

	//ゲームオブジェクトマネージャでしか使わない。(どうにかすべき)
	const ComponentManager& GetComponentManager()
	{
		return _Components;
	}

	//オブジェクトのアクティブフラグを設定する　セッター
	virtual void SetActive(const bool& act)
	{
		_Active = act;
	}

	//アクティブかどうか取得　ゲッター
	const bool& GetActive()
	{
		return _Active;
	}

	void Discard(const bool& b)
	{
		_Discard = b;
		
		//for each (Transform* t in transform->GetChildren())
		//{
		//	//子に全てに親と同じ値を設定
		//	t->gameObject->Discard(_Discard);
		//}
	}

	const bool& Discard()
	{
		return _Discard;
	}
public:
	//トランスフォーム(簡単にアクセスしたかった。)
	Transform* transform;	
protected:
	//コンポーネントたち
	ComponentManager _Components;
	//アクティブでないオブジェクトは描画もアップデートもされない
	bool _Active;
	//オブジェクトを破棄する
	bool _Discard;
};