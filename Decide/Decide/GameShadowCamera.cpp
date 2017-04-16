#include "GameShadowCamera.h"
#include "fbEngine/ShadowCamera.h"

void GameShadowCamera::Awake()
{
	ShadowCamera* camera = AddComponent<ShadowCamera>();
	INSTANCE(GameObjectManager)->mainShadowCamera = camera;

	transform->SetLocalPosition(Vector3(-500, 760, -460));
	transform->SetLocalAngle(Vector3(52, 48, 0));
	camera->SetNear(1);
	camera->SetFar(1500);
	camera->SetViewVolume(Vector2(1200, 1200));
}

void GameShadowCamera::Update()
{
	//í≤êÆóp
	ShadowCamera* camera = GetComponent<ShadowCamera>();
	static Vector2 nf = { 1,1500 };
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

	//camera->SetNear(nf.x);
	//camera->SetFar(nf.y);
	
	Vector3 lang = Vector3::zero;
	if (KeyBoardInput->isPressed(DIK_LEFT))
	{
		lang.y -= 1;
	}
	if (KeyBoardInput->isPressed(DIK_RIGHT))
	{
		lang.y += 1;
	}
	if (KeyBoardInput->isPressed(DIK_UP))
	{
		lang.x -= 1;
	}
	if (KeyBoardInput->isPressed(DIK_DOWN))
	{
		lang.x += 1;
	}
	Vector3 la = transform->GetLocalAngle();
	//transform->SetLocalAngle(la + (lang));

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

	Vector3 lp = transform->GetLocalPosition();
	//transform->SetLocalPosition(lp + transform->Direction(dir));
}