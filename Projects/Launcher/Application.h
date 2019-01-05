#pragma once
#include "Tools.h"
#include "Options.h"

class Application {
public:
	Application();
	~Application();
private:
	Environment m_env;
	Devices m_dev;
	Options m_opt;
public:
	void ProcessUserInput();
	int Update(int timeout = 1500);
	int Run();
	void Print();
};