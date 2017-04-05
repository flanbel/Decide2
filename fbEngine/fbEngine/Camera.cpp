#include "Camera.h"
#include "EffectManager.h"

void Camera::Awake()
{
	//行列初期化
	D3DXMatrixIdentity(&_View);
	D3DXMatrixIdentity(&_Projection);

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
	//逆行列取得
	D3DXMatrixInverse(&_View, NULL, &transform->GetWorldMatrix());
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