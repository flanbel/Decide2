#include "GameShadowCamera.h"
#include "fbEngine/ShadowCamera.h"

void GameShadowCamera::Awake()
{
	ShadowCamera* camera = AddComponent<ShadowCamera>();
	GameObjectManager::mainShadowCamera = camera;

	/*transform->SetLocalPosition(Vector3(-1270, 3100, -1800);
	transform->localAngle = Vector3(69, 49, 0);
	camera->Near(3040);
	camera->Far(4000);*/

	transform->SetLocalPosition(Vector3(-2044, 2034, -2344));
	transform->SetLocalAngle(Vector3(50, 50, 0));
	camera->Near(2544);
	camera->Far(4310);
	//camera->Near(1);
	//camera->Far(2000);
}

void GameShadowCamera::Update()
{
	//í≤êÆóp
	ShadowCamera* camera = GetComponent<ShadowCamera>();
	static Vector2 nf = { 2544,4310 };
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

	/*camera->Near(nf.x);
	camera->Far(nf.y);


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
	if (KeyBoardInput->isPressed(DIK_W))
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