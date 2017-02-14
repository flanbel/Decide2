#include "GameObject.h"

GameObject::GameObject(char* Name)
{
	//自分を登録
	active = true;
	name = Name;

	transform = AddComponent<Transform>();
}

GameObject::~GameObject()
{
	components.Release();
}

//void GameObject::Active(bool act)
//{
//	Object::Active(act);
//	//子が居る
//	if (transform->ChildCnt() > 0)
//	{
//		//いったん取り出す
//		map<char*, Transform*> child = transform->Children();
//		//イテレータ取得
//		map<char*, Transform*>::iterator it = child.begin();
//		//終わりまでループ
//		while (it != child.end())
//		{
//			//子を同じ状態にする
//			it->second->gameObject->active = act;
//			it++;
//		}
//	}
//}