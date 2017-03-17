#pragma once
#include "Camera.h" 

class ShadowCamera :public Camera
{
public:
	ShadowCamera(GameObject* g, Transform* t) :Camera(g, t, typeid(this).name())
	{
		
	}
	void Update()override;
	//•½s“Š‰e
	//‚­‚Á‚»’·‚¢ŠÖ”–¼
	void ParallelProjectionMatrixUpdate()
	{
		float w = _near * tan(D3DXToRadian(viewAngle) / 2) * 2;
		float h = w * aspect;
		//‰e‚Í•½s“Š‰e‚ÅÊ‚µ‚½‚¢‚È‚è
		//•½s“Š‰e‚Å‚µ‚È‚¢‚È‚ç‰e‚ÍL‚Ñ‚é
		D3DXMatrixOrthoLH(
			&projection,
			w,
			h,
			_near,
			_far);
	};
};