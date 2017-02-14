#include "FPS.h"
#include "Text.h"

void FPS::Start()
{
	text = AddComponent<Text>();
	
	text->SetSize(40.0f);
	text->SetString(L"FPS:0");
}

void FPS::Update()
{
	wchar_t fps[30];
	wchar_t dnum[10];
	FloattoString(Time::Fps(), dnum,2);
	wcscpy_s(fps, wcslen(L"FPS:")+1, L"FPS:");
	wcscat_s(fps, wcslen(fps) + wcslen(dnum)+1, dnum);
	text->SetString(fps);
}