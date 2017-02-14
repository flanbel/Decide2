#include "ComponentManager.h"

ComponentManager::~ComponentManager()
{
	Release();
}

void ComponentManager::Start()
{
	vector<Component*>::iterator it = components.begin();
	while (it != components.end())
	{
		if ((*it)->GetActive())
		{
			(*it)->Start();
		}
		it++;
	}
}

void ComponentManager::Update()
{
	

	vector<Component*>::iterator it = components.begin();
	while (it != components.end())
	{
		if ((*it)->GetActive())
		{
			(*it)->Update();
		}
		it++;
	}
}

void ComponentManager::LateUpdate()
{
	vector<Component*>::iterator it = components.begin();
	while (it != components.end())
	{
		if ((*it)->GetActive())
		{
			(*it)->LateUpdate();
		}
		it++;
	}
}

void ComponentManager::PreRender()
{
	vector<Component*>::iterator it = components.begin();
	while (it != components.end())
	{
		if ((*it)->GetActive())
		{
			(*it)->PreRender();
		}
		it++;
	}
}

void ComponentManager::Render()
{
	vector<Component*>::iterator it = components.begin();
	while (it != components.end())
	{
		if ((*it)->GetActive())
		{
			(*it)->Render();
		}
		it++;
	}
}

void ComponentManager::PostRender()
{
	vector<Component*>::iterator it = components.begin();
	while (it != components.end())
	{
		if ((*it)->GetActive())
		{
			(*it)->PostRender();
		}
		it++;
	}
}

void ComponentManager::ImageRender()
{
	vector<Component*>::iterator it = components.begin();
	while (it != components.end())
	{
		if ((*it)->GetActive())
		{
			(*it)->ImageRender();
		}
		it++;
	}
}

Component* ComponentManager::GetComponent(char * Componentname)
{
	char classname[64];
	strcpy_s(classname, Length("class "), "class ");
	strcat_s(classname, Length(classname) + Length(Componentname), Componentname);

	vector<Component*>::iterator it = components.begin();
	while (it != components.end())
	{
		if (strcmp((*it)->Name(), classname) == 0)
			return *it;
		it++;
	}

	return nullptr;
}

void ComponentManager::Release()
{
	vector<Component*>::iterator it = components.begin();
	while (it != components.end())
	{
		//ƒƒ‚ƒŠ‰ğ•ú
		SAFE_DELETE(*it);
		//ƒ}ƒbƒv‚©‚çíœ
		it = components.erase(it);
	}
}