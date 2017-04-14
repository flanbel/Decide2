#include "Camera.h"
#include "EffectManager.h"

Camera::~Camera()
{
	SetViewPoint(nullptr);
}

void Camera::Awake()
{
	//行列初期化
	D3DXMatrixIdentity(&_View);
	D3DXMatrixIdentity(&_Projection);

	_ViewPoint = nullptr;
	_ViewAngle = 45;
	_Aspect = (16.0f / 9.0f);
	_near = 1.0f;
	_far = 1000.0f;
}

void Camera::Start()
{
	
}

void Camera::Update()
{
	ViewMatrixUpdate();
	ProjectionMatrixUpdate();
}

void Camera::ViewMatrixUpdate()
{
	if (_ViewPoint)
	{
		//注視点を使う
		D3DXMatrixLookAtLH(&_View, (D3DXVECTOR3*)&transform->GetPosition(), (D3DXVECTOR3*)_ViewPoint, (D3DXVECTOR3*)&transform->Direction(Vector3::up));
	}
	else
	{
		//逆行列を使う
		D3DXMatrixInverse(&_View, NULL, &transform->GetWorldMatrix());
	}
}

void Camera::ProjectionMatrixUpdate()
{
	// プロジェクション変換(射影変換)
	D3DXMatrixPerspectiveFovLH(
		&_Projection,
		D3DXToRadian(_ViewAngle),	//画角
		_Aspect,		//アスペクト比
		_near,			//クリップ距離(Near)
		_far);			//クリップ距離(far)
}

Vector2 Camera::WorldToScreen(Vector3 wpos)
{
	//ビューポート行列
	D3DXMATRIX viewport;
	D3DXMatrixIdentity(&viewport);
	viewport._11 = g_WindowSize.x / 2;
	viewport._22 = -g_WindowSize.y / 2;
	viewport._41 = g_WindowSize.x / 2;
	viewport._42 = g_WindowSize.y / 2;
	D3DXVECTOR4 v = { wpos.x,wpos.y,wpos.z,1 };
	D3DXVec4Transform(&v, &v, &_View);
	D3DXVec4Transform(&v, &v, &_Projection);
	//各要素をwで割る
	v.x /= v.w;
	v.y /= v.w;
	v.z /= v.w;
	v.w /= v.w;
	D3DXVec4Transform(&v, &v, &viewport);

	return Vector2(v.x, v.y);
}

Vector3 Camera::ScreenToWorld(Vector2 spos)
{
	D3DXMATRIX viewportInv, projInv, viewInv;
	D3DXMatrixIdentity(&viewportInv);
	viewportInv._11 = g_WindowSize.x / 2;
	viewportInv._22 = -g_WindowSize.y / 2;
	viewportInv._41 = g_WindowSize.x / 2;
	viewportInv._42 = g_WindowSize.y / 2;
	//逆行列に
	D3DXMatrixInverse(&viewportInv, NULL, &viewportInv);
	D3DXMatrixInverse(&projInv, NULL, &_Projection);
	D3DXMatrixInverse(&viewInv, NULL, &_View);
	//2D座標に逆行列をかけていく。
	D3DXVECTOR4 v = { spos.x,spos.y,0,1 };
	D3DXVec4Transform(&v, &v, &viewportInv);
	D3DXVec4Transform(&v, &v, &projInv);
	D3DXVec4Transform(&v, &v, &viewInv);

	return Vector3(v.x, v.y, v.z);
}