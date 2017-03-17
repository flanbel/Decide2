#pragma once
#include "Object.h"

//前方定義
class GameObject;
class Transform;

//コンポーネントの基底クラス

class Component : public Object
{
public:
	Component(GameObject* g, Transform* t,const char* classname):Object(classname)
	{
		enable = true;
		gameObject = g;
		transform = t;
	}
	virtual ~Component() {};
	

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
};