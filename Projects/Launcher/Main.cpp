#include "Common.h"
#include "Tools.h"
#include "Application.h"

int __stdcall wWinMain(HINSTANCE, HINSTANCE, LPWSTR, INT) {
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	auto res = make_unique<Application>()->Run();
#ifdef _DEBUG
	system("pause");
#endif
	return res;
}