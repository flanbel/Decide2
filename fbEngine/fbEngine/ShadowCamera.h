#pragma once
#include "Camera.h" 

class ShadowCamera :public Camera
{
public:
	ShadowCamera(GameObject* g, Transform* t) :
		Camera(g, t, typeid(this).name()),
		_ViewVolume(Vector2(500,500))
	{
		
	}
	void Update()override;
	//•½s“Š‰e
	void ParallelProjectionMatrixUpdate()
	{
		float w = 3000.0f;// _near * tan(D3DXToRadian(_ViewAngle) / 2) * 2;
		float h = 3000.0f;// w * _Aspect;
		//•½s“Š‰e‚ğ‚·‚é‚Æ‰e‚ªL‚Ñ‚È‚­‚È‚é
		D3DXMatrixOrthoLH(
			&_Projection,
			_ViewVolume.x,
			_ViewVolume.y,
			_near,
			_far);
	};
	void SetViewVolume(Vector2 v)
	{
		_ViewVolume = v;
	}
protected:
	//•½s“Š‰e‚ğ‚·‚éê‡‚Í‰æŠp‚ªŠÖŒW‚È‚­‚È‚éB‚È‚Ì‚Å‚±‚¢‚Â‚ÅÊ‚·”ÍˆÍ‚ğw’è‚·‚éB
	Vector2 _ViewVolume;
};