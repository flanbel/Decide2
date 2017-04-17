#include "ShadowCamera.h"

void ShadowCamera::Update()
{
	ViewMatrixUpdate();
	ParallelProjectionMatrixUpdate();
}
