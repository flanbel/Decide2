#include "GameObject.h"

GameObject::GameObject():
	Object(),
	_Active(true),
	_Discard(true)
{
	transform = AddComponent<Transform>();
}

GameObject::GameObject(const char* name):
	Object(name),
	_Active(true),
	_Discard(true)
{
	transform = AddComponent<Transform>();
}

GameObject::~GameObject()
{
	_Components.Release();
}

//void GameObject::Active(bool act)
//{
//	Object::Active(act);
//	//子が居る
//	if (transform->ChildCnt() > 0)
//	{
//		//いったん取り出す
//		map<char*, transform*> child = transform->Children();
//		//イテレータ取得
//		map<char*, transform*>::iterator it = child.begin();
//		//終わりまでループ
//		while (it != child.end())
//		{
//			//子を同じ状態にする
//			it->second->gameObject->_Active = act;
//			it++;
//		}
//	}
//}