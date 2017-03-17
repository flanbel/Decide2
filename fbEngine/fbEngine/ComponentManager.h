#pragma once
#include "Component.h"

//コンポーネントの管理
class ComponentManager :Noncopyable
{
public:
	~ComponentManager();
	void Start()const;
	void Update()const;
	void LateUpdate()const;
	void PreRender()const;
	void Render()const;
	void PostRender()const;
	void ImageRender()const;

	//コンポーネント型のみを追加したい・・・。
	template<class T>
	T* AddComponent(GameObject* g,Transform* t)
	{
		T* component = new T(g, t);
		_Components.push_back(component);
		component->Awake();
		return component;
	}

	//不要？？？
	void AddComponent(Component* c)
	{
		_Components.push_back(c);
	}

	Component* GetComponent(const char* name);

	template <class T>
	T* GetComponent();

	template <class T>
	void RemoveComponent()
	{
		char* name = (char*)typeid(T).name();

		vector<Component*>::iterator it = _Components.begin();
		while (it != _Components.end())
		{
			if (strcmp((*it)->GetName(), name) == 0)
			{
				SAFE_DELETE(*it);
				_Components.erase(it);
				return;
			}
			it++;
		}
	}
	//全てのコンポーネントの解放
	void Release();
private:
	//コンポーネントたち
	vector<Component*> _Components;
};

template<class T>
inline T * ComponentManager::GetComponent()
{
	//テンプレート型の内部の型情報取得
	const type_info& Ttype = typeid(T);

	//foreachは内部で値をコピーしているので、ポインタを返すなら使えない。
	vector<Component*>::const_iterator it = _Components.cbegin();
	while (it != _Components.end())
	{
		//コンポーネント型の内部の型情報取得
		const type_info& type = typeid(*(*it));
		//型情報比較
		if (Ttype == type)
		{
			return (T*)*it;
		}
		it++;
	}

	return nullptr;
}