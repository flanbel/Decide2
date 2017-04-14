#include "GameObjectManager.h"
#include "GameObject.h"

vector<list<GameObject*>> GameObjectManager::_GameObjects;
list<GameObjectManager::RemoveObj> GameObjectManager::_RemoveList;
Camera* GameObjectManager::mainCamera = nullptr;
Light* GameObjectManager::mainLight = nullptr;
ShadowCamera* GameObjectManager::mainShadowCamera = nullptr;

GameObject* GameObjectManager::Add(GameObject* pAdd, int priority)
{
	if(_GameObjects.size() <= 0)
	{
		_GameObjects.resize(System::MAX_PRIORITY);
	}
	GameObject* obj = pAdd;
	_GameObjects.at(priority).push_back(obj);
	obj->Awake();
	return pAdd;
}

void GameObjectManager::StartObject()
{
	for (short priority = 0; priority < System::MAX_PRIORITY; priority++)
	{
		for each (GameObject* obj in _GameObjects[priority])
		{
			if (obj->GetActive())
			{
				obj->Start();
				obj->GetComponentManager().Start();
			}
		}
	}
}

void GameObjectManager::UpdateObject()
{
	//削除リストを削除
	_RemoveObject();

	for (short priority = 0; priority < System::MAX_PRIORITY; priority++)
	{
		for each (GameObject* obj in _GameObjects[priority])
		{
			if (obj->GetActive())
			{
				obj->Update();
				obj->GetComponentManager().Update();
			}
		}
	}
}

void GameObjectManager::LateUpdateObject()
{
	for (short priority = 0; priority < System::MAX_PRIORITY; priority++)
	{
		for each (GameObject* obj in _GameObjects[priority])
		{
			if (obj->GetActive())
			{
				obj->LateUpdate();
				obj->GetComponentManager().LateUpdate();
			}
		}
	}
}

void GameObjectManager::PreRenderObject()
{
	//レンダーターゲットセット
	for (short priority = 0; priority < System::MAX_PRIORITY; priority++)
	{
		for each (GameObject* obj in _GameObjects[priority])
		{
			if (obj->GetActive())
			{
				obj->PreRender();
				obj->GetComponentManager().PreRender();
			}
		}
	}
}

void GameObjectManager::RenderObject()
{
	for (short priority = 0; priority < System::MAX_PRIORITY; priority++)
	{
		for each (GameObject* obj in _GameObjects[priority])
		{
			if (obj->GetActive())
			{
				obj->Render();
				obj->GetComponentManager().Render();
			}
		}
	}
}

void GameObjectManager::PostRenderObject()
{
	for (short priority = 0; priority < System::MAX_PRIORITY; priority++)
	{
		for each (GameObject* obj in _GameObjects[priority])
		{
			if (obj->GetActive())
			{
				obj->PostRender();
				obj->GetComponentManager().PostRender();
			}
		}
	}
}

void GameObjectManager::ImageRenderObject()
{
	for (short priority = 0; priority < System::MAX_PRIORITY; priority++)
	{
		for each (GameObject* obj in _GameObjects[priority])
		{
			if (obj->GetActive())
			{
				obj->ImageRender();
				obj->GetComponentManager().ImageRender();
			}
		}
	}
}

void GameObjectManager::AddRemoveList(GameObject * obj)
{
	for (short priority = 0; priority < System::MAX_PRIORITY; priority++)
	{
		list<GameObject*>::iterator it = _GameObjects[priority].begin();

		while (it != _GameObjects[priority].end())
		{
			//アドレスの比較
			if (obj == *it)
			{
				RemoveObj remove(it, priority);
				_RemoveList.push_back(remove);
				return;
			}
			else
				it++;
		}
	}
}

void GameObjectManager::AddRemoveList(char * name)
{
	for (short priority = 0; priority < System::MAX_PRIORITY; priority++)
	{
		list<GameObject*>::iterator it = _GameObjects[priority].begin();

		while (it != _GameObjects[priority].end())
		{
			//名前の比較
			if (strcmp(name, (*it)->GetName()) == 0)
			{
				RemoveObj remove(it,priority);
				_RemoveList.push_back(remove);
				return;
			}
			else
				it++;
		}
	}
}

GameObject* GameObjectManager::FindObject(char* name)
{
	for (short priority = 0; priority < System::MAX_PRIORITY; priority++)
	{
		list<GameObject*>::iterator it = _GameObjects[priority].begin();

		while (it != _GameObjects[priority].end())
		{
			//名前の比較
			if (strcmp(name, (*it)->GetName()) == 0)
			{
				return (*it);
			}
			else
				it++;
		}
	}
	//見つからなかった時はnullptrを返却
	return nullptr;
}

bool GameObjectManager::FindObjects(char* name, GameObject ** objArray)
{
	for (short priority = 0; priority < System::MAX_PRIORITY; priority++)
	{
		list<GameObject*>::iterator it = _GameObjects[priority].begin();

		while (it != _GameObjects[priority].end())
		{
			//名前の比較
			if (strcmp(name, (*it)->GetName()) == 0)
			{
				objArray[0] = (*it);
			}
			else
				it++;
		}
	}
	return false;
}

void GameObjectManager::_RemoveObject()
{
	for each (RemoveObj remove in _RemoveList)
	{
		SAFE_DELETE(*remove.it);
		_GameObjects[remove.prio].erase(remove.it);
	} 
	_RemoveList.clear();
}

void GameObjectManager::Release()
{
	for (short priority = 0; priority < System::MAX_PRIORITY; priority++)
	{
		if (_GameObjects.size() > 0)
		{
			list<GameObject*>::iterator it = _GameObjects[priority].begin();
			//すべて追加
			while (it != _GameObjects[priority].end())
			{
				if ((*it)->Discard())
				{
					RemoveObj remove(it, priority);
					_RemoveList.push_back(remove);
				}
				else
				{
					//破棄しない
					//次は破棄するようにする
					(*it)->Discard(true);
				}

				it++;
			}
		}
	}
	_RemoveObject();
	mainCamera = nullptr;
	mainLight = nullptr;
}
