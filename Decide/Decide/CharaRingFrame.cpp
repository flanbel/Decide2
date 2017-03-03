#include "CharaRingFrame.h"
#include "fbEngine/ImageObject.h"
#include "fbEngine/CircleCollision.h"
#include "fbEngine/Sprite.h"

void CharaRingFrame::Awake()
{
	ImageObject* icon = GameObjectManager::AddNew<ImageObject>("icon", 0);
	ring = GameObjectManager::AddNew<ImageObject>("frame", 0);

	ring->transform->SetParent(transform);
	TEXTURE* ringtex = LOADTEXTURE("ring.png");
	ring->SetTexture(ringtex);
	ring->SetClipColor(Color::red);

	circle = ring->AddComponent<CircleCollision>();
	circle->SetRadius(ringtex->size.x/2);

	icon->SetTexture(LOADTEXTURE("nico.png"));
	icon->transform->SetParent(ring->transform);
}

void CharaRingFrame::Start()
{
}

void CharaRingFrame::Update()
{
}

bool CharaRingFrame::Judgment(Vector2 pos)
{
	return circle->Judgment(pos);
}
