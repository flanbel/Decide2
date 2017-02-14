#pragma once
#include "GameObject.h"

class Sprite;
//画像を表示するゲームオブジェクト
class ImageObject :public GameObject
{
public:
	ImageObject(char* name) :GameObject(name) {}
	void Awake()override;
	void SetTexture(TEXTURE* t);
	TEXTURE* GetTexture();
	void SetBlendColor(Color c);
	void SetClipColor(Color c);
private:
	Sprite* sprite;
};
