#include "Common.h"
#include "Application.h"

Application::Application()
{
	winrt::init_apartment();
#ifdef _DEBUG
	Tools::Instance()->OpenConsole();
#endif
}

Application::~Application()
{
	winrt::uninit_apartment();
}

void Application::ProcessUserInput() {
	while (!m_dev.m_pad.StopRequested()) {
		m_dev.m_pad.ReadControllerState();

		if (m_dev.m_pad.IsPressed(XINPUT_GAMEPAD_A)) {
			printf("Button is pressed			\n");
		}
		//if (m_dev.m_pad.WasPressed(XINPUT_GAMEPAD_A)) {
		//	printf("Button was pressed			\n");
		//}

		Tools::Instance()->SleepFor(150);
	}
}

int Application::Update(int timeout)
{
	if (m_opt.gamepad) {
		thread t(&Application::ProcessUserInput, this);
		m_dev.m_pad.Run(t);
	}
	do {
		GetExitCodeProcess(m_env.m_fileInfo.targetAppHandle, &m_env.m_fileInfo.targetProcessStatus);
		if (timeout > 0) {
			Tools::Instance()->SleepFor(timeout);
		}
	} while (m_env.m_fileInfo.targetProcessStatus == STILL_ACTIVE);

	if (m_opt.gamepad) {
		m_dev.m_pad.Stop();
	}
	return false;
}

int Application::Run() {
	m_opt.Init(m_env, m_dev);

	if (!m_opt.multiInstance) {
		m_env.m_fileInfo.currentAppHandle = CreateMutex(0, TRUE, m_env.m_fileInfo.exeName.c_str());
		if (GetLastError() == ERROR_ALREADY_EXISTS)
			return false;
	}

	if (m_opt.bootExplorer)
		m_env.m_systemInfo.BootProcess(L"explorer.exe", m_env.m_systemInfo.winDir);

	if (m_opt.downsampling || m_opt.customFrequency)
		m_dev.m_display.SetResolution();

	if (m_env.OpenAppById(m_env.m_fileInfo.exeName)) {
#ifdef _DEBUG
		Print();
#endif

		if (m_opt.forceFullscreen) {
			m_dev.m_kbd.VirtualWinShiftEnter();
		}

		return Update(1500);
	}
	
	m_env.GenerateExecutables();
#ifdef _DEBUG
	system("pause");
#endif
	return false;
}

void Application::Print()
{
	m_env.Print();
	m_opt.Print();
	//m_dev.Print();
}
