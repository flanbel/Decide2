#pragma once
#include "Component.h"

//コンポーネントの管理
class ComponentManager :Noncopyable
{
public:
	~ComponentManager();
	void Start();
	void Update();
	void LateUpdate();
	void PreRender();
	void Render();
	void PostRender();
	void ImageRender();

	//基本使うな
	template<class T>
	T* AddComponent(GameObject* g,Transform* t)
	{
		T* C = new T(g, t);
		components.push_back(C);
		C->Awake();
		return C;
	}

	void AddComponent(Component* c)
	{
		components.push_back(c);
	}

	Component* GetComponent(char* name);

	template <class T>
	T* GetComponent()
	{
		char* name = (char*)typeid(T).name();

		vector<Component*>::iterator it = components.begin();
		while (it != components.end())
		{
			if (strcmp((*it)->Name(), name) == 0)
				return (T*)*it;
			it++;
		}

		return nullptr;
	}

	template <class T>
	void RemoveComponent()
	{
		char* name = (char*)typeid(T).name();

		vector<Component*>::iterator it = components.begin();
		while (it != components.end())
		{
			if (strcmp((*it)->Name(), name) == 0)
			{
				SAFE_DELETE(*it);
				components.erase(it);
				return;
			}
			it++;
		}
	}

	void Release();
private:
	//コンポーネントたち
	vector<Component*> components;
};