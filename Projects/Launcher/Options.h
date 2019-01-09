#pragma once
#include "Environment.h"
#include "Devices.h"

class Options
{
private:
	void ExtractId(wstring);
public:
	bool refocusOnShutdown{ true };
	bool multiInstance{ false };
	bool bootExplorer{ false };
	bool downsampling{ false };
	bool boundDownsampling{ false };
	bool customFrequency{ false };
	bool gamepad{ false };
	bool forceFullscreen{ false };
	bool supressError{ false };
	//bool steamApp{ false };
public:
	void Init(Environment& env, Devices& dev);
	void Print();
};

