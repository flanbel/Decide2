#include "GameCamera.h"
#include "fbEngine/Camera.h"

void GameCamera::Awake()
{
	Camera* camera = AddComponent<Camera>();
	GameObjectManager::mainCamera = camera;
	transform->SetLocalPosition(Vector3(0, 991, -684));
	transform->SetLocalAngle(Vector3(60, 0, 0));
	camera->Near(10);
	camera->Far(1850);
	//camera->Near(1);
	//camera->Far(2000);
}

void GameCamera::Update()
{
	//í≤êÆóp
	Camera* camera = GetComponent<Camera>();
	static Vector2 nf = { 10,1850 };
	if (KeyBoardInput->isPressed(DIK_N))
	{
		nf.x++;
	}
	if (KeyBoardInput->isPressed(DIK_M))
	{
		nf.x--;
	}
	if (KeyBoardInput->isPressed(DIK_F))
	{
		nf.y++;
	}
	if (KeyBoardInput->isPressed(DIK_G))
	{
		nf.y--;
	}

	//camera->Near(nf.x);
	//camera->Far(nf.y);
	/*

	if (KeyBoardInput->isPressed(DIK_LEFT))
	{
		transform->localAngle.y -= 1;
	}
	if (KeyBoardInput->isPressed(DIK_RIGHT))
	{
		transform->localAngle.y += 1;
	}
	if (KeyBoardInput->isPressed(DIK_UP))
	{
		transform->localAngle.x -= 1;
	}
	if (KeyBoardInput->isPressed(DIK_DOWN))
	{
		transform->localAngle.x += 1;
	}*/

	Vector3 dir = Vector3::zero;
	if(KeyBoardInput->isPressed(DIK_W))
	{
		dir.z += 1;
	}
	if (KeyBoardInput->isPressed(DIK_S))
	{
		dir.z -= 1;
	}
	if (KeyBoardInput->isPressed(DIK_A))
	{
		dir.x -= 1;
	}
	if (KeyBoardInput->isPressed(DIK_D))
	{
		dir.x += 1;
	}
	if (KeyBoardInput->isPressed(DIK_O))
	{
		dir.y += 1;
	}
	if (KeyBoardInput->isPressed(DIK_L))
	{
		dir.y -= 1;
	}

	//transform->localPosition.Add(transform->Direction(dir));
}
