#include "GameShadowCamera.h"
#include "fbEngine/ShadowCamera.h"

void GameShadowCamera::Awake()
{
	ShadowCamera* camera = AddComponent<ShadowCamera>();
	GameObjectManager::mainShadowCamera = camera;

	transform->localPosition = Vector3(-270, 515, -700);
	transform->localAngle = Vector3(40, 23, 0);
	camera->Near(500);
	camera->Far(1500);
	//camera->Near(1);
	//camera->Far(2000);
}

void GameShadowCamera::Update()
{
	//í≤êÆóp
	/*ShadowCamera* camera = GetComponent<ShadowCamera>();
	static Vector2 nf = { 500,1500 };
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

	camera->Near(nf.x);
	camera->Far(nf.y);*/
	/*

	if (KeyBoardInput->isPressed(DIK_LEFT))
	{
	transform->localAngle.y -= 3;
	}
	if (KeyBoardInput->isPressed(DIK_RIGHT))
	{
	transform->localAngle.y += 3;
	}
	if (KeyBoardInput->isPressed(DIK_UP))
	{
	transform->localAngle.x -= 3;
	}
	if (KeyBoardInput->isPressed(DIK_DOWN))
	{
	transform->localAngle.x += 3;
	}*/

	Vector3 dir = Vector3::zero;
	if (KeyBoardInput->isPressed(DIK_W))
	{
		dir.z += 3;
	}
	if (KeyBoardInput->isPressed(DIK_S))
	{
		dir.z -= 3;
	}
	if (KeyBoardInput->isPressed(DIK_A))
	{
		dir.x -= 3;
	}
	if (KeyBoardInput->isPressed(DIK_D))
	{
		dir.x += 3;
	}
	if (KeyBoardInput->isPressed(DIK_O))
	{
		dir.y += 3;
	}
	if (KeyBoardInput->isPressed(DIK_L))
	{
		dir.y -= 3;
	}

	//transform->localPosition.Add(transform->Direction(dir));
}
