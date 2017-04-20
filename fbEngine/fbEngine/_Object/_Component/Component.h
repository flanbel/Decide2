#pragma once
#include "_Object\Object.h"

class GameObject;
class Transform;

class Component : public Object
{
public:
	Component(GameObject* g, Transform* t,const char* classname,int order = 1):Object(classname)
	{
		enable = true;
		gameObject = g;
		transform = t;
		_ExecutionOrder = order;
	}
	virtual ~Component() {};

	bool operator<(const Component& right)
	{
		return (this->_ExecutionOrder > right._ExecutionOrder);
	}	

	/*virtual void Awake() {};
	virtual void Start() {};
	virtual void Update() {};
	virtual void LateUpdate() {};
	virtual void PreRender() {};
	virtual void Render() {};*/

	//コンポーネントを有効にする
	bool enable;
	//自分がアタッチしているゲームオブジェクトのアドレス
	GameObject* gameObject;
	//gameObjectのトランスフォームのアドレス
	Transform* transform;
private:
	//実行順番(実行時にしか設定できない)
	int _ExecutionOrder;
};