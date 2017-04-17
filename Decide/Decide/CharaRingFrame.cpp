#include "CharaRingFrame.h"

#include "fbEngine\_Object\_GameObject\ImageObject.h"
#include "fbEngine\_Object\_Component\_2D\CircleCollision.h"

void CharaRingFrame::Awake()
{
	ImageObject* icon = INSTANCE(GameObjectManager)->AddNew<ImageObject>("icon", 0);
	ring = INSTANCE(GameObjectManager)->AddNew<ImageObject>("frame", 0);

	ring->transform->SetParent(transform);
	TEXTURE* ringtex = LOADTEXTURE("ring.png");
	ring->SetTexture(ringtex);
	ring->SetClipColor(Color::red);

	circle = ring->AddComponent<CircleCollision>();
	circle->SetRadius(ringtex->Size.x/2);

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
