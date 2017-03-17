#include "Camera.h"
#include "EffectManager.h"

void Camera::Awake()
{
	//行列初期化
	D3DXMatrixIdentity(&view);
	D3DXMatrixIdentity(&projection);

	viewAngle = 45;
	aspect = (16.0f / 9.0f);
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
	//逆行列取得
	D3DXMatrixInverse(&view, NULL, &transform->GetWorldMatrix());
}

void Camera::ProjectionMatrixUpdate()
{
	// プロジェクション変換(射影変換)
	D3DXMatrixPerspectiveFovLH(
		&projection,
		D3DXToRadian(viewAngle),	//画角
		aspect,		//アスペクト比
		_near,			//クリップ距離(Near)
		_far);			//クリップ距離(far)
}