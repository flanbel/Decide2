#pragma once
#include "GameObject.h"
#include "_Object\_Component\_2D\Text.h"

//FPS•\Ž¦
class FPS:public GameObject
{
public:
	FPS(char* name):GameObject(name)
	{
	
	}
	void Start()override;
	void Update()override;
private:
	Text* _Text;
};