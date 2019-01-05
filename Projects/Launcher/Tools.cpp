#include "Common.h"
#include "Tools.h"

unique_ptr<Tools> Tools::m_pInstance = nullptr;

typedef int(__stdcall *MSGBOXWAPI)(IN HWND hWnd,
	IN LPCWSTR lpText, IN LPCWSTR lpCaption,
	IN UINT uType, IN WORD wLanguageId, IN DWORD dwMilliseconds);

Tools* Tools::Instance()
{
	if (!m_pInstance) {
		m_pInstance.reset(new Tools());
	}
	return m_pInstance.get();
}

void Tools::OpenConsole()
{
	if (!m_bConsoleStatus) {
		m_bConsoleStatus = true;

		AllocConsole();
		FILE *pCout, *pCin;
		freopen_s(&pCin, "CONIN$", "r", stdin);
		freopen_s(&pCout, "CONOUT$", "w", stdout);
		freopen_s(&pCout, "CONOUT$", "w", stderr);
	}
}

void Tools::CloseConsole()
{
	if (m_bConsoleStatus) {
		m_bConsoleStatus = false;

		FreeConsole();
	}
}

void Tools::SleepFor(int millisecond)
{
	sleep_for(10ns);
	sleep_until(system_clock::now() + std::chrono::milliseconds(millisecond));
}

void Tools::DebugOutput(wstring desc, wstring msg)
{
#ifdef _DEBUG
	printf("DEBUG: %ls | %ls",desc.c_str(), msg.c_str());
#endif
}

int Tools::MessageBoxTimeout(LPCWSTR message, DWORD _ms, LPCWSTR title, UINT type, HWND hWnd, WORD langId )
{
	HMODULE hUser32 = LoadLibrary(_T("user32.dll"));
	static MSGBOXWAPI MsgBoxTOW = NULL;

	if (!MsgBoxTOW)
	{
		HMODULE hUser32t = GetModuleHandle(_T("user32.dll"));
		if (hUser32t)
		{
			MsgBoxTOW = (MSGBOXWAPI)GetProcAddress(hUser32t,
				"MessageBoxTimeoutW");
		}
		else
		{
			FreeLibrary(hUser32);
			return 0;
		}
	}

	if (MsgBoxTOW)
	{
		return MsgBoxTOW(hWnd, message, title,
			type, langId, _ms);
	}
	FreeLibrary(hUser32);
	return 0;
}

wstring Tools::RemoveForbiddenChars(wstring src)
{
	wstring forbiddenChars = L"\\/:?\"<>|";
	wstring out = src;
	for (auto it = out.begin(); it < out.end(); ++it) {
		bool found = forbiddenChars.find(*it) != wstring::npos;
		if (found)
			*it = ' ';
		out.erase(remove(out.begin(), out.end(), ' '), out.end());
	}
	return out;
}
