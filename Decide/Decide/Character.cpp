#include "Character.h"
#include "fbEngine/Animation.h"
#include "fbEngine/SkinModelData.h"
CharacterInfo::~CharacterInfo()
{
	SAFE_DELETE(Data);
	SAFE_DELETE(Anim);
}
