#include "ComponentManager.h"

ComponentManager::~ComponentManager()
{
	Release();
}

void ComponentManager::Start() const
{
	for each (Component* c in _Components)
	{
		if (c->enable)
		{
			c->Start();
		}
	}
}

void ComponentManager::Update() const
{
	for each (Component* c in _Components)
	{
		if (c->enable)
		{
			c->Update();
		}
	}
}

void ComponentManager::LateUpdate() const
{
	for each (Component* c in _Components)
	{
		if (c->enable)
		{
			c->LateUpdate();
		}
	}
}

void ComponentManager::PreRender() const
{
	for each (Component* c in _Components)
	{
		if (c->enable)
		{
			c->PreRender();
		}
	}
}

void ComponentManager::Render() const
{
	for each (Component* c in _Components)
	{
		if (c->enable)
		{
			c->Render();
		}
	}
}

void ComponentManager::PostRender() const
{
	for each (Component* c in _Components)
	{
		if (c->enable)
		{
			c->PostRender();
		}
	}
}

void ComponentManager::ImageRender() const
{
	for each (Component* c in _Components)
	{
		if (c->enable)
		{
			c->ImageRender();
		}
	}
}

Component* ComponentManager::GetComponent(const char * Componentname)
{
	char classname[64];
	strcpy_s(classname, strlen("class ")+1, "class ");
	strcat_s(classname, strlen(classname) + strlen(Componentname)+1, Componentname);

	vector<Component*>::const_iterator it = _Components.begin();
	while (it != _Components.end())
	{
		if (strcmp((*it)->GetName(), classname) == 0)
			return *it;
		it++;
	}

	return nullptr;
}

void ComponentManager::Release()
{
	vector<Component*>::iterator it = _Components.begin();
	while (it != _Components.end())
	{
		//メモリ解放
		SAFE_DELETE(*it);
		//マップから削除、次に進める
		it = _Components.erase(it);
	}
}