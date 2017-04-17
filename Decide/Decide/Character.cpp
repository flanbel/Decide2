#include "Character.h"

CharacterInfo::~CharacterInfo()
{
	SAFE_DELETE(Data);
	SAFE_DELETE(Anim);
}
