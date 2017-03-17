#pragma once
#include "fbEngine/Scene.h"
class SoundSource;

class TitleScene : public Scene
{
public:
	TitleScene() {};
	void Start()override;
	void Update()override;
private:
	//Œø‰Ê‰¹
	SoundSource *_EnterSE;
};