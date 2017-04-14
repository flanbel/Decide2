#include "GameCamera.h"
#include "fbEngine/Camera.h"

void GameCamera::Awake()
{
	camera = AddComponent<Camera>();
	GameObjectManager::mainCamera = camera;
	transform->SetLocalPosition(Vector3(0, 0, 0));
	transform->SetLocalAngle(Vector3(70, 0, 0));
	camera->SetNear(1);
	camera->SetFar(1500);
}

void GameCamera::Update()
{
	//調整用
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
	if (KeyBoardInput->isPressed(DIK_DOWN))
	{
		lang.x -= 1;
	}
	if (KeyBoardInput->isPressed(DIK_UP))
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

	static float a = 1.7;
	if (KeyBoardInput->isPressed(DIK_UP))
	{
		a -= 0.01;
	}
	if (KeyBoardInput->isPressed(DIK_DOWN))
	{
		a += 0.01;
	}
	//camera->SetAspect(a);

	//画角設定
	static float angle = 45.0f;
	if (KeyBoardInput->isPressed(DIK_O))
	{
		angle -= 1;
	}
	if (KeyBoardInput->isPressed(DIK_L))
	{
		angle += 1;
	}
	camera->SetViewAngle(angle);

	_UpdatePos();
	_UpdateViewAngle();
}

void GameCamera::AddPlayer(Player * p)
{
	_PlayerList.push_back(p);
}
#include "Player.h"
void GameCamera::_UpdateViewAngle()
{
	//角度
	float angle = 0.0f;
	for each (Player* p in _PlayerList)
	{
		Vector3 ppos = p->transform->GetPosition();
		Vector3 pos = transform->GetPosition();

		//カメラからプレイヤーへのベクトル計算
		Vector2 toP = Vector2(ppos.x - pos.x, ppos.z - pos.z);
		Vector2 foward = Vector2(transform->Direction(Vector3::front).x, transform->Direction(Vector3::front).z);
		toP.Normalize();
		foward.Normalize();
		//カメラの前向きのベクトルと内積をとる
		//角度が大きいなら更新
		float dot = toP.Dot(foward) - 1.0f - D3DXToRadian(15);
		angle = max(angle, fabs(dot));
	}
	//画角設定
	camera->SetViewAngle(D3DXToDegree(angle)*2);
}

void GameCamera::_UpdatePos()
{
	Vector3 Min, Max;
	Min = Vector3(9999, 9999, 9999);
	Max = Vector3(-9999, -9999, -9999);

	for each (Player* p in _PlayerList)
	{
		Min.x = min(Min.x, p->transform->GetPosition().x);
		Min.y = min(Min.y, p->transform->GetPosition().y);
		Min.z = min(Min.z, p->transform->GetPosition().z);

		Max.x = max(Max.x, p->transform->GetPosition().x);
		Max.y = max(Max.y, p->transform->GetPosition().y);
		Max.z = max(Max.z, p->transform->GetPosition().z);
	}

	//平均ポジションを出す
	Vector3 average = Min + Max;
	average.Div(2);

	//注視点設定
	Vector3 vp = camera->GetViewPoint();
	//線形補完する
	vp.Lerp(average, 1, Time::DeltaTime());
	camera->SetViewPoint(vp);

	static float dist = 500.0f;
	/*if (KeyBoardInput->isPressed(DIK_O))
	{
		dist -= 3;
	}
	if (KeyBoardInput->isPressed(DIK_L))
	{
		dist += 3;
	}*/

	//視点設定
	Vector3 p = Vector3(average.x, average.y, Min.z) + (transform->Direction(Vector3::back) * dist);
	Vector3 lp = transform->GetLocalPosition();
	//線形補完
	lp.Lerp(p, 0.5, Time::DeltaTime());
	transform->SetLocalPosition(lp);
}
