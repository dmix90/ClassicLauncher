#pragma once
#include "Tools.h"

struct FileInfo {
	~FileInfo() {
		ReleaseMutexAndHandle();
	};
	wstring fileDir;
	wstring fileName;
	wstring exeName;
	HANDLE currentAppHandle{ nullptr };
	HANDLE targetAppHandle{ nullptr };
	ulong targetProcessId{ 0 };
	ulong targetProcessStatus{ 0 };
	ulong currentProcessId{GetCurrentProcessId()};
	ulong currentProcessStatus{ STILL_ACTIVE };
	vector<wstring> launchArgs;
	void ReleaseMutexAndHandle() {
		if (currentAppHandle) {
			ReleaseMutex(currentAppHandle);
			CloseHandle(currentAppHandle);
		}
	};
	void Print() {
		printf("FileDir: %ls\nFileName: %ls\nExecutableName: %ls\nLaunchArgs:\nCurrentProcessId: %i\n", 
			fileDir.c_str(), fileName.c_str(), exeName.c_str(), currentProcessId);
		for (uint i = 0; i < launchArgs.size(); i++) {
			printf("\tArg[%i]: %ls\n", i, launchArgs[i].c_str());
		}
		printf("%ls\n", wstring(60, '-').c_str());
	}
};
struct SystemInfo {
	~SystemInfo() {
		KillAllProcesses();
	};
	wstring winDir;
	map<wstring, PROCESS_INFORMATION> processMap;
	bool IsProcessRunning(wstring pName) {
		auto snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (snapshot == INVALID_HANDLE_VALUE) {
#ifdef _DEBUG
			printf("IsProcessRunning: INVALID_HANDLE_VALUE");
#endif
			return false;
		}

		PROCESSENTRY32 procEntry;
		procEntry.dwSize = sizeof(PROCESSENTRY32);

		if (!Process32First(snapshot, &procEntry))
		{
#ifdef _DEBUG
			printf("IsProcessRunning: Could not retrieve first process");
#endif
			return false;
		}

		do {
			if (pName == procEntry.szExeFile) {
#ifdef _DEBUG
				printf("IsProcessRunning: Found process[%ls]\n", pName.c_str());
#endif
				return true;
			}
		} while (Process32Next(snapshot, &procEntry));

#ifdef _DEBUG
		printf("IsProcessRunning: Process[%ls] was not found\n", pName.c_str());
#endif
		return false;
	};
	bool BootProcess(wstring pName, wstring fromPath = L"", wstring args = L"") {
		wstring bootstrap = fromPath + pName;

		STARTUPINFO startupInfo;
		ZeroMemory(&startupInfo, sizeof(startupInfo));

		PROCESS_INFORMATION processInfo;
		ZeroMemory(&processInfo, sizeof(processInfo));

		if (CreateProcess(bootstrap.c_str(), &args[0], 0, 0, 0, 0, 0, 0, &startupInfo, &processInfo)) {
			printf("BootProcess: Successfully launched process[%ls] from [%ls]\n", pName.c_str(), fromPath.c_str());
			processMap[pName] = processInfo;
			return true;
		}
		printf("BootProcess: Failed to boot process[%ls] from [%ls]\n", pName.c_str(), fromPath.c_str());
		return false;
	};
	bool KillProcess(wstring pName) {
		if (processMap.find(pName) != processMap.end()) {
			TerminateProcess(processMap[pName].hProcess, 1 );
			CloseHandle(processMap[pName].hProcess);
			printf("KillProcess: Killed process[%ls]\n", pName.c_str());
			return true;
		}
		printf("KillProcess: Failed to kill process[%ls]\n", pName.c_str());
		return false;
	};
	void KillAllProcesses() {
		for (auto&& proc : processMap) {
			KillProcess(proc.first);
		}
	};
	void Print() {
		printf("WinDir: %ls\n", winDir.c_str());
		printf("%ls\n", wstring(60, '-').c_str());
	};
};
struct PackageInfo {
	wstring name;
	wstring description;
	wstring propName;
	wstring id;
	wstring appendix;
	wstring location;
	wstring bootstrap;
	wstring test;
};

class Environment
{
private:
	void ExtractFileLaunchArgs();
	void ExtractFilePathVars();
	void ExtractSystemPathVars();
	long ActivateModernApp(IApplicationActivationManager* aam, wstring id, wstring args = L"") {
		auto hr = aam->ActivateApplication(id.c_str(), &args[0], AO_NONE, &m_fileInfo.targetProcessId);
		if (SUCCEEDED(hr)) {
			printf("OpenAppById: Activate application success [%ls]\n", id.c_str());
			m_fileInfo.targetAppHandle = OpenProcess(PROCESS_ALL_ACCESS, false, m_fileInfo.targetProcessId);
			GetExitCodeProcess(m_fileInfo.targetAppHandle, &m_fileInfo.targetProcessStatus);
		}
		else
			printf("OpenAppById: Activate application failure [%ls]\n", id.c_str());

		return hr;
	};
public:
	FileInfo m_fileInfo;
	SystemInfo m_systemInfo;
	vector<PackageInfo> m_packages;
public:
	void Init();
	void Print();
	int OpenAppById(wstring id, wstring args = L"") {
		CComPtr<IApplicationActivationManager> aam{ nullptr };

		auto hr = CoCreateInstance(CLSID_ApplicationActivationManager, nullptr, CLSCTX_LOCAL_SERVER, IID_PPV_ARGS(&aam));
		if (SUCCEEDED(hr)) {
			printf("OpenAppById: CoCreateInstance success\n");

			hr = CoAllowSetForegroundWindow(aam, nullptr);
			if (SUCCEEDED(hr)) {
				hr = ActivateModernApp(aam, id, args);
				if (SUCCEEDED(hr))
					return true;
				else {
					if (m_fileInfo.launchArgs.size() >= 2) {
						hr = ActivateModernApp(aam, m_fileInfo.launchArgs[1].c_str(), args);
						if (SUCCEEDED(hr))
							return true;
					}
					return false;
				}
			}
			else
				printf("OpenAppById: CoAllowSetForegroundWindow failure\n");
		}
		else
			printf("OpenAppById: CoCreateInstance failure\n");
		return false;
	}
	bool AdminRightsAvailable() {
		auto token{ GetCurrentProcessToken() };
		TOKEN_ELEVATION	tokenEl{};

		if (token) {
			DWORD tlSize = sizeof(tokenEl);
			GetTokenInformation(token, TokenElevation, &tokenEl, sizeof(tokenEl), &tlSize);
			CloseHandle(token);
		}

		return static_cast<bool>(tokenEl.TokenIsElevated);
	};
	void GenerateExecutables(bool filter = false) {
		PackageManager packageManager;

		for (auto&& package : packageManager.FindPackagesForUserWithPackageTypes(hstring{ L"" }, PackageTypes::Main)) {
			printf("%ls\n", package.Id().Name().c_str());
			if (!package.IsDevelopmentMode() || package.Status().VerifyIsOK()) {
				PackageId packageId = package.Id();
				try {
					StorageFolder storageFolder = package.InstalledLocation();
					StorageFile storageFile = storageFolder.GetFileAsync(L"AppxManifest.xml").get();
					XmlDocument doc = XmlDocument::LoadFromFileAsync(storageFile).get();

					PackageInfo packageTemp;
					if (doc.GetElementsByTagName(L"uap:VisualElements").Length() > 0) {
						packageTemp.name = doc.GetElementsByTagName(L"uap:VisualElements").Item(0).Attributes().GetNamedItem(L"DisplayName").InnerText().c_str();
						packageTemp.description = doc.GetElementsByTagName(L"uap:VisualElements").Item(0).Attributes().GetNamedItem(L"Description").InnerText().c_str();
					}
					else {
						packageTemp.name = L"N/A";
						packageTemp.description = L"N/A";
					}

					if (doc.GetElementsByTagName(L"DisplayName").Length() > 0) {
						packageTemp.propName = doc.GetElementsByTagName(L"DisplayName").Item(0).InnerText().c_str();
					}
					else if (doc.GetElementsByTagName(L"f:DisplayName").Length() > 0) {
						packageTemp.propName = doc.GetElementsByTagName(L"f:DisplayName").Item(0).InnerText().c_str();
					}
					else {
						packageTemp.propName = L"N/A";
					}

					if (doc.GetElementsByTagName(L"Application").Length() > 0) {
						packageTemp.appendix = doc.GetElementsByTagName(L"Application").Item(0).Attributes().GetNamedItem(L"Id").InnerText().c_str();
					}
					else if (doc.GetElementsByTagName(L"f:Application").Length()>0) {
						packageTemp.appendix = doc.GetElementsByTagName(L"f:Application").Item(0).Attributes().GetNamedItem(L"Id").InnerText().c_str();
					}
					else {
						break;
					}
					packageTemp.id = packageId.FamilyName().c_str();
					packageTemp.location = storageFolder.Path().c_str();
					packageTemp.bootstrap = packageTemp.id + L"!" + packageTemp.appendix;
					packageTemp.test = storageFolder.Name();

					if (filter) {
						if (packageTemp.propName.find(L"ms-resource:") == wstring::npos || packageTemp.name.find(L"ms-resource:") == wstring::npos) {
							m_packages.push_back(packageTemp);
						};
					}
					else
						m_packages.push_back(packageTemp);
				}
				catch (hresult_error const& ex) {
					auto hr = ex.to_abi();
					switch (hr) {

					case HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND): {
						wstring pkgName = packageId.FullName().c_str();
						wstring msg = L"[" + pkgName + L"] Corrupted package detected. Application data was not found. Do you want to remove app?";
						if (MessageBox(0, msg.c_str(), L"Package error", MB_OKCANCEL | MB_ICONEXCLAMATION) == IDOK) {
							auto deploymentOperation{ packageManager.RemovePackageAsync(packageId.FullName()) };
							deploymentOperation.get();
							if (deploymentOperation.Status() == AsyncStatus::Error) {

							}
						}
						break;
					}
					default:
						break;
					}
				}
			}
		}

		for (const auto& package : m_packages) {
			printf("Name: %ls\nDescription: %ls\nPropertyName: %ls\nId: %ls\nAppendix: %ls\nLocation: %ls\nBootstrap: %ls\n-\nTest:%ls\n",
				package.name.c_str(),
				package.description.c_str(),
				package.propName.c_str(),
				package.id.c_str(),
				package.appendix.c_str(),
				package.location.c_str(),
				package.bootstrap.c_str(),
				package.test.c_str()
			);
		}
		wstring outDir = L"Apps\\";
		CreateDirectory(outDir.c_str(), nullptr);
		for (auto&& package : m_packages) {
			wstring outFile = outDir + package.bootstrap + L".exe";
			if (CopyFile(m_fileInfo.fileName.c_str(), outFile.c_str(), FALSE)) {
				printf("Successfully copied file\n");
			}
			else {
				printf("Could not create file in current directory\n");
			}
		}
	}
};

