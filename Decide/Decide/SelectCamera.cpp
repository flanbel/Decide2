#include "SelectCamera.h"
#include "fbEngine/Camera.h"

void SelectCamera::Awake()
{
	Camera* camera = AddComponent<Camera>();
	INSTANCE(GameObjectManager)->mainCamera = camera;
	transform->SetLocalPosition(Vector3(0, 50, -150));
	transform->SetLocalAngle(Vector3(0, 0, 0));
	camera->SetNear(1);
	camera->SetFar(300);
}

void SelectCamera::Update()
{
	//í≤êÆóp
	/*Camera* camera = GetComponent<Camera>();
	static Vector2 nf = { 500,1500 };
	if (KeyBoardInput->isPush(DIK_N))
	{
	nf.x++;
	}
	if (KeyBoardInput->isPush(DIK_M))
	{
	nf.x--;
	}
	if (KeyBoardInput->isPush(DIK_F))
	{
	nf.y++;
	}
	if (KeyBoardInput->isPush(DIK_G))
	{
	nf.y--;
	}

	camera->Near(nf.x);
	camera->Far(nf.y);

	if (KeyBoardInput->isPush(DIK_LEFT))
	{
	transform->localAngle.y -= 3;
	}
	if (KeyBoardInput->isPush(DIK_RIGHT))
	{
	transform->localAngle.y += 3;
	}
	if (KeyBoardInput->isPush(DIK_UP))
	{
	transform->localAngle.x -= 3;
	}
	if (KeyBoardInput->isPush(DIK_DOWN))
	{
	transform->localAngle.x += 3;
	}*/

	Vector3 dir = Vector3::zero;
	if (KeyBoardInput->isPush(DIK_W))
	{
		dir.z += 3;
	}
	if (KeyBoardInput->isPush(DIK_S))
	{
		dir.z -= 3;
	}
	if (KeyBoardInput->isPush(DIK_A))
	{
		dir.x -= 3;
	}
	if (KeyBoardInput->isPush(DIK_D))
	{
		dir.x += 3;
	}
	if (KeyBoardInput->isPush(DIK_O))
	{
		dir.y += 3;
	}
	if (KeyBoardInput->isPush(DIK_L))
	{
		dir.y -= 3;
	}

	//transform->localPosition.Add(transform->Direction(dir));
}
