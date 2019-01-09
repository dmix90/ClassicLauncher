#include "Common.h"
#include "Options.h"

void Options::Init(Environment& env, Devices& dev)
{
	env.Init();
	dev.m_display.Init();

	!env.m_systemInfo.IsProcessRunning(L"explorer.exe") ? bootExplorer = true : 0;

	for (const auto arg : env.m_fileInfo.launchArgs) {
		if (arg.find(L"ds:") != wstring::npos) {
			boundDownsampling = true;
			downsampling = true;		
			dev.m_display.ExtractBoundResolution(arg);
		}
		if (arg.find(L"cf:") != wstring::npos) {
			customFrequency = true;
			dev.m_display.ExtractCustomFrequencyValue(arg);
		}
		if (arg == L"pad") {
			dev.m_pad.Init();
			gamepad = true;
		}
		//if (arg.find(L"id:") != wstring::npos) {
		//	steamApp = true;
		//	ExtractId(arg);
		//}
		arg == L"ds"	? downsampling = true : 0;
		arg == L"ffs"	? forceFullscreen = true : 0;
		arg == L"mi"	? multiInstance = true : 0;
		arg == L"norf"	? refocusOnShutdown = false : 0;
		arg == L"f"		? supressError = true : 0;
	}
}

void Options::Print()
{
	printf("Options:\n");
	printf("\tRefocusOnShutdown: %s\n", refocusOnShutdown ? "true" : "false");
	printf("\tMultiInstance: %s\n", multiInstance ? "true" : "false");
	printf("\tBootExplorer: %s\n", bootExplorer ? "true" : "false");
	printf("\tDownsampling: %s\n", downsampling ? "true" : "false");
	printf("\tBoundDownsampling: %s\n", boundDownsampling ? "true" : "false");
	printf("\tCustomFrequency: %s\n", customFrequency ? "true" : "false");
	printf("\tGamepad: %s\n", gamepad ? "true" : "false");
	printf("\tForceFullscreen: %s\n", forceFullscreen ? "true" : "false");
	printf("\tSupressError: %s\n", supressError ? "true" : "false");
	printf("%ls\n", wstring(60, '-').c_str());
}

void Options::ExtractId(wstring arg) {
	wstring id = arg.substr(arg.find_last_of(L"id:") + 1);
	printf("Id arg: %ls\n", id.c_str());
}