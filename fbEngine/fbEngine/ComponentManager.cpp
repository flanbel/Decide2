#include "ComponentManager.h"

ComponentManager::~ComponentManager()
{
	Release();
}

void ComponentManager::Start() const
{
	vector<Component*>::const_iterator it = _Components.begin();
	while (it != _Components.end())
	{
		if ((*it)->enable)
		{
			(*it)->Start();
		}
		it++;
	}
}

void ComponentManager::Update() const
{
	

	vector<Component*>::const_iterator it = _Components.begin();
	while (it != _Components.end())
	{
		if ((*it)->enable)
		{
			(*it)->Update();
		}
		it++;
	}
}

void ComponentManager::LateUpdate() const
{
	vector<Component*>::const_iterator it = _Components.begin();
	while (it != _Components.end())
	{
		if ((*it)->enable)
		{
			(*it)->LateUpdate();
		}
		it++;
	}
}

void ComponentManager::PreRender() const
{
	vector<Component*>::const_iterator it = _Components.begin();
	while (it != _Components.end())
	{
		if ((*it)->enable)
		{
			(*it)->PreRender();
		}
		it++;
	}
}

void ComponentManager::Render() const
{
	vector<Component*>::const_iterator it = _Components.begin();
	while (it != _Components.end())
	{
		if ((*it)->enable)
		{
			(*it)->Render();
		}
		it++;
	}
}

void ComponentManager::PostRender() const
{
	vector<Component*>::const_iterator it = _Components.begin();
	while (it != _Components.end())
	{
		if ((*it)->enable)
		{
			(*it)->PostRender();
		}
		it++;
	}
}

void ComponentManager::ImageRender() const
{
	vector<Component*>::const_iterator it = _Components.begin();
	while (it != _Components.end())
	{
		if ((*it)->enable)
		{
			(*it)->ImageRender();
		}
		it++;
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