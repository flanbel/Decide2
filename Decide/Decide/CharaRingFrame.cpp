#include "CharaRingFrame.h"
#include "fbEngine/ImageObject.h"
#include "fbEngine/CircleCollision.h"
#include "fbEngine/Sprite.h"

void CharaRingFrame::Awake()
{
	ImageObject* icon = GameObjectManager::AddNew<ImageObject>("icon", 0);
	ring = GameObjectManager::AddNew<ImageObject>("frame", 0);

	ring->transform->SetParent(transform);
	ring->SetTexture(TextureManager::LoadTexture("ring.png"));
	ring->SetClipColor(Color::red);
	circle = ring->AddComponent<CircleCollision>();
	TEXTURE* t = ring->GetComponent<Sprite>()->GetTexture();
	circle->SetRadius(t->size.x/2);
	t->pivot = Vector2(0.5, 0.5);
	TEXTURE* i = TextureManager::LoadTexture("nico.png");
	i->pivot = Vector2(0.5, 0.5);
	icon->SetTexture(i);
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
