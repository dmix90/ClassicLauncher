#include "Common.h"
#include "Environment.h"

void Environment::ExtractFileLaunchArgs()
{
	int numArgs{ 0 };
	auto argsTemp = CommandLineToArgvW(GetCommandLineW(), &numArgs);

	for (auto i = 0; i < numArgs; i++) {
		wstring arg = argsTemp[i];
		m_fileInfo.launchArgs.push_back(arg.substr(arg.find_last_of('-') + 1));
	}
}

void Environment::ExtractFilePathVars()
{
	wstring temp(MAX_PATH, '\0');
	GetModuleFileNameW(nullptr, &temp[0], static_cast<long>(temp.length()));
	temp.resize(wcslen(&temp[0]));

	m_fileInfo.fileDir	= temp.substr(0, temp.find_last_of('\\') + 1);
	m_fileInfo.fileName = temp.substr(temp.find_last_of('\\') + 1);
	m_fileInfo.exeName	= m_fileInfo.fileName.substr(0, m_fileInfo.fileName.find_last_of('.'));

	if (std::all_of(m_fileInfo.exeName.begin(), m_fileInfo.exeName.end(), ::isdigit)) {
		m_fileInfo.exeName = L"steam://rungameid/" + m_fileInfo.exeName;
		printf("All is digit: %ls\n", m_fileInfo.exeName.c_str());
	}
}

void Environment::ExtractSystemPathVars()
{
	wstring temp(MAX_PATH, '\0');
	GetWindowsDirectory(&temp[0], static_cast<long>(temp.length()));
	temp.resize(wcslen(&temp[0]));

	m_systemInfo.winDir = temp + L"\\";
}

void Environment::Init()
{
	ExtractFileLaunchArgs();
	ExtractFilePathVars();
	ExtractSystemPathVars();
}

void Environment::Print()
{
	m_fileInfo.Print();
	m_systemInfo.Print();
}
