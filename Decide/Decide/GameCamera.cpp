#include "GameCamera.h"
#include "fbEngine\_Object\_Component\_3D\Camera.h"

void GameCamera::Awake()
{
	camera = AddComponent<Camera>();
	INSTANCE(GameObjectManager)->mainCamera = camera;
	transform->SetLocalPosition(Vector3(0, 0, 0));
	transform->SetLocalAngle(Vector3(70, 0, 0));
	camera->SetNear(1);
	camera->SetFar(1500);

	camera->SetViewPoint(Vector3(0, 200, 0));
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
	//camera->SetViewAngle(angle);

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
		if (p->GetAlive())
		{
			//カメラ座標系に変換
			D3DXMATRIX view;
			D3DXMatrixInverse(&view, NULL, &transform->GetWorldMatrix());
			D3DXVECTOR4 player;
			//座標をカメラ座標系に変換
			D3DXVec3Transform(&player, (D3DXVECTOR3*)&p->transform->GetPosition(), &view);

			float xzD, yzD;
			//x/zでtangentθを計算して、atanでθを求める。
			xzD = atan(player.x / player.z);
			yzD = atan(player.y / player.z);
			//大きい方をとる。
			float dot = max(xzD, yzD);
			angle = max(angle, dot);
		}
	}
	float theta = D3DXToDegree(angle) * 2;
	theta = max(40, theta);	//下限
	theta = min(89, theta);	//上限
	//画角設定
	camera->SetViewAngle(theta);
}

void GameCamera::_UpdatePos()
{
	Vector3 Min, Max;
	Min = Vector3(9999, 9999, 9999);
	Max = Vector3(-9999, -9999, -9999);
	//最低値と最大値を計算する。
	for each (Player* p in _PlayerList)
	{
		if (p->GetAlive())
		{
			Min.x = min(Min.x, p->transform->GetPosition().x);
			Min.y = min(Min.y, p->transform->GetPosition().y);
			Min.z = min(Min.z, p->transform->GetPosition().z);

			Max.x = max(Max.x, p->transform->GetPosition().x);
			Max.y = max(Max.y, p->transform->GetPosition().y);
			Max.z = max(Max.z, p->transform->GetPosition().z);
		}
	}

	//平均ポジションを出す
	Vector3 average = (Min + Max) / 2.0f;
	//0の時
	if(!(average.Length() > 0.0f))
	{
		//中心を見る。
		average = Vector3(0, 130, 0);
		Min.z = 0.0f;
	}
	//-500より後ろには向かない
	average.z = max(average.z, -500.0f);

	static Vector3 pre = Vector3::zero;

	Vector3 currpos = camera->GetViewPoint();//現在位置
	Vector3 trgpos = average;//ターゲット位置
	Vector3 prevtrgpos = pre;//直前のターゲットの位置
	float deltatime = Time::DeltaTime();//時間変化
	float springConst = 2.0f;//フックの定数
	float dampConst = 2.0f;//ダンピング定数
	float springLen = 10;//バネの長さ？

	Vector3 disp;		//変位
	Vector3 velocity;	//速度
	float forceMag;		//力の大きさ
	
	//バネの力計算
	disp = currpos - trgpos;
	if (disp.Length() > 0.0f)
	{
		Vector3 dot = disp;//内積用
		velocity = (prevtrgpos - trgpos) * deltatime;
		forceMag = springConst * (springLen - disp.Length()) + dampConst*(dot.Dot(velocity) / disp.Length());

		//バネの力を適用
		disp.Normalize();
		disp = disp * forceMag * deltatime;
		currpos += disp;
	}
	pre = trgpos;
	//注視点設定
	camera->SetViewPoint(currpos);

	static float dist = 500.0f;
	//視点設定
	Vector3 p = Vector3(currpos.x, currpos.y, Min.z) + (transform->Direction(Vector3::back) * dist);
	transform->SetLocalPosition(p);
}