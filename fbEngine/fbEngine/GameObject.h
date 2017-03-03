#pragma once
#include "Object.h"
#include "ComponentManager.h"

class Component;

//ゲームオブジェクトの基礎クラス
class GameObject : public Object
{
public:
	GameObject();
	GameObject(char* name);
	virtual ~GameObject();

	/*virtual void Awake() {};
	virtual void Start() {};
	virtual void Update() {};
	virtual void LateUpdate() {};
	virtual void PreRender() {};
	virtual void Render() {};*/

	template<class T>
	T* AddComponent()
	{
		return components.AddComponent<T>(this,transform);
	}

	void AddComponent(Component* c)
	{
		return components.AddComponent(c);
	}

	Component* GetComponent(char* name)
	{
		return components.GetComponent(name);
	}

	template <class T>
	T* GetComponent()
	{
		return components.GetComponent<T>();
	}

	ComponentManager& GetComponentManager()
	{
		return components;
	}

	//トランスフォーム
	Transform* transform;

	//オブジェクトのアクティブフラグを設定する　セッター
	virtual void SetActive(bool act)
	{
		active = act;
	}

	//アクティブかどうか取得　ゲッター
	bool GetActive()
	{
		return active;
	}

	void Discard(bool b)
	{
		discard = b;
		//子にも設定
		
	}

	bool Discard()
	{
		return discard;
	}
protected:
	//コンポーネントたち
	ComponentManager components;
	//アクティブでないオブジェクトは描画もアップデートもされない
	bool active;
	//オブジェクトを破棄する
	bool discard;
};