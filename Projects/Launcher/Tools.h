#pragma once
#include "Common.h"

class Tools
{
private:
	static unique_ptr<Tools> m_pInstance;
public:
	static Tools* Instance();
	Tools(const Tools&) = delete;
	Tools& operator=(Tools&) = delete;
	Tools() {};
	~Tools() { CloseConsole(); };
private:
	bool m_bConsoleStatus{ false };
public:
	void OpenConsole();
	void CloseConsole();
	void SleepFor(int millisecond);
	void DebugOutput(wstring desc, wstring msg);
	int MessageBoxTimeout(LPCWSTR message, DWORD ms = 2000, LPCWSTR title = L"Message", UINT type = MB_OK, HWND hWnd = 0, WORD langId = 0);
	wstring RemoveForbiddenChars(wstring);
};

#define DEBUG_OUT(desc, msg) { Tools::Instance()->DebugOutput(desc, msg);}

class StoppableTask {
private:
	promise<void> exitSignal;
	future<void> futureObj{ exitSignal.get_future() };
	thread t;
public:
	StoppableTask() {};
	StoppableTask(StoppableTask&& obj) noexcept :
		exitSignal(move(obj.exitSignal)), futureObj(move(obj.futureObj)) {
		printf("StoppableTask move constructor is called");
	};
	StoppableTask& operator=(StoppableTask& obj) {
		printf("StoppableTask move assignment is called");
		exitSignal = move(obj.exitSignal);
		futureObj = move(obj.futureObj);
		return *this;
	}
	virtual ~StoppableTask() {};
public:
	virtual void Run(thread& _t) {
		t = move(_t);
	}
	/*void operator()() { Run(); };*/
	bool StopRequested() {
		if (futureObj.wait_for(std::chrono::milliseconds(1)) == std::future_status::timeout)
			return false;
		return true;
	}
	void Stop() {
		exitSignal.set_value();
		if (t.joinable())
			t.join();
	}
	thread& Thread() { return t; }
};