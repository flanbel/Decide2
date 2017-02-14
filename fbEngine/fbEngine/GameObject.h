#pragma once
#include "Object.h"
#include "ComponentManager.h"

class Component;

//ゲームオブジェクトの基礎クラス
class GameObject : public Object
{
public:
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
protected:
	//コンポーネントたち
	ComponentManager components;
};