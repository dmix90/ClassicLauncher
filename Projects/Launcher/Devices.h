#pragma once
#include "Tools.h"

struct Resolution {
	ulong width;
	ulong height;
	ulong frequency;
	bool operator==(const Resolution& obj) {
		if (this->width == obj.width && this->height == obj.height && this->frequency == obj.frequency)
			return true;
		return false;
	}
	bool operator!=(const Resolution& obj) {
		if (*this == obj)
			return false;
		return true;
	}
};

struct Display {
	~Display() {
		ResetResolution();
	};
	DEVMODE m_devmode;
	vector<Resolution> m_resList;
	Resolution m_originalRes{ 0,0,0 };
	Resolution m_maxRes{ 0,0,0 };
	Resolution m_minRes{ 0,0,0 };
	Resolution m_boundRes{ 0,0,0 };
	int customFrequencyValue{ 0 };
	void ExtractBoundResolution(wstring arg) {
		wstring res		= arg.substr(arg.find_first_of(L":") + 1);
		wstring width	= res.substr(0, res.find_first_of('x'));
		wstring height	= res.substr(res.find_first_of('x') + 1);

		m_boundRes.width		= stoi(width);
		m_boundRes.height		= stoi(height);
		m_boundRes.frequency	= m_originalRes.frequency;
	};
	void ExtractCustomFrequencyValue(wstring arg) {
		wstring frequency		= arg.substr(arg.find_first_of(L":") + 1);

		customFrequencyValue	= stoi(frequency);
	}
	bool GetOriginalResolution() {
		if (EnumDisplaySettings(0, ENUM_CURRENT_SETTINGS, &m_devmode)) {
			m_originalRes.width		= m_devmode.dmPelsWidth;
			m_originalRes.height	= m_devmode.dmPelsHeight;
			m_originalRes.frequency = m_devmode.dmDisplayFrequency;
			return true;
		};
		printf("Could not get original display resolution!");
		return false;
	};
	bool QueryAllAvailableResolutionPairs() {
		if (GetOriginalResolution()) {
			vector<ulong> tempWidth;
			vector<ulong> tempHeight;
			for (auto i = 0; EnumDisplaySettings(0, i, &m_devmode); i++) {
				m_resList.push_back({ m_devmode.dmPelsWidth, m_devmode.dmPelsHeight, m_devmode.dmDisplayFrequency });
				tempWidth.push_back(m_devmode.dmPelsWidth);
				tempHeight.push_back(m_devmode.dmPelsHeight);
			}
			ulong maxWidth	= *max_element(tempWidth.begin(), tempWidth.end());
			ulong maxHeight = *max_element(tempHeight.begin(), tempHeight.end());
			ulong minWidth	= *min_element(tempWidth.begin(), tempWidth.end());
			ulong minHeight = *min_element(tempHeight.begin(), tempHeight.end());

			m_maxRes = { maxWidth, maxHeight, m_originalRes.frequency };
			m_minRes = { minWidth, minHeight, m_originalRes.frequency };

			return  true;
		};

		return false;
	};
	bool ResetResolution() {
		if (ChangeResolution(m_originalRes)) {
			printf("Successfully restored original display resolution!");
			return true;
		}
		printf("Could not restore original display resolution!");
		return false;
	};
	bool SetResolution() {
		if (m_boundRes != Resolution{ 0,0,0 }) {
			return ChangeResolution(m_boundRes);
		}
		else if (m_boundRes == m_originalRes) {
			return true;
		}
		else {
			return ChangeResolution(m_maxRes);
		}
	};
	bool ChangeResolution(const Resolution& res) {
		if (res.width > m_maxRes.width || res.height > m_maxRes.height || res.frequency > m_maxRes.frequency)
			return false;
		if (res.width < m_minRes.width || res.height < m_minRes.height || res.frequency < m_minRes.frequency)
			return false;
		if (m_devmode.dmPelsWidth == res.width || m_devmode.dmPelsHeight == res.height || m_devmode.dmDisplayFrequency == res.frequency)
			return true;

		m_devmode.dmPelsWidth			= res.width;
		m_devmode.dmPelsHeight			= res.height;
		m_devmode.dmDisplayFrequency	= res.frequency;
		if (ChangeDisplaySettings(&m_devmode, CDS_FULLSCREEN) == DISP_CHANGE_SUCCESSFUL)
		{
			Tools::Instance()->SleepFor(3500);
				return true;
		}
		Tools::Instance()->MessageBoxTimeout(L"Failed to set display resolution to specified bounds", 5000, L"Error", MB_ICONERROR);

		return ResetResolution();
	}
	void Init() {
		QueryAllAvailableResolutionPairs();
		if (customFrequencyValue != 0) {
			m_boundRes.frequency = customFrequencyValue;
			m_maxRes.frequency = customFrequencyValue;
		}
	};
	void Print() {
		printf("Available resolutions: \n");
		for (auto&& res : m_resList) {
			printf("\t %ix%i:%ihz\n", res.width, res.height, res.frequency);
		}
		printf("Original display resolution: \n\tWidth: %i\n\tHeight: %i\n", m_originalRes.width, m_originalRes.height);
		printf("Maximum available resolution: \n\tWidth: %i\n\tHeight: %i\n", m_maxRes.width, m_maxRes.height);
		printf("Bound maximum resolution: \n\tWidth: %i\n\tHeight: %i\n", m_boundRes.width, m_boundRes.height);
		printf("Custom frequency value: %i\n", customFrequencyValue);
	};
};

struct Keyboard {
	void VirtualWinShiftEnter() {
		keybd_event(VK_LWIN, 0, 0, 0);
		keybd_event(VK_SHIFT, 0, 0, 0);	
		keybd_event(VK_RETURN, 0, 0, 0);
		keybd_event(VK_RETURN, 0, KEYEVENTF_KEYUP, 0);
		keybd_event(VK_SHIFT, 0, KEYEVENTF_KEYUP, 0);
		keybd_event(VK_LWIN, 0, KEYEVENTF_KEYUP, 0);
	}
};

using XInputGetStateEx_t = ulong(__stdcall*)(ulong userIndex, XINPUT_STATE* inputState);
constexpr int XINPUT_GAMEPAD_GUIDE = 0x400;
constexpr array gamepadButtons = {
	XINPUT_GAMEPAD_GUIDE,
	XINPUT_GAMEPAD_DPAD_UP,
	XINPUT_GAMEPAD_DPAD_DOWN,
	XINPUT_GAMEPAD_DPAD_LEFT,
	XINPUT_GAMEPAD_DPAD_RIGHT,
	XINPUT_GAMEPAD_START,
	XINPUT_GAMEPAD_BACK,
	XINPUT_GAMEPAD_LEFT_THUMB,
	XINPUT_GAMEPAD_RIGHT_THUMB,
	XINPUT_GAMEPAD_LEFT_SHOULDER,
	XINPUT_GAMEPAD_RIGHT_SHOULDER,
	XINPUT_GAMEPAD_A,
	XINPUT_GAMEPAD_B,
	XINPUT_GAMEPAD_X,
	XINPUT_GAMEPAD_Y,
};

enum ButtonState {JustPressed, JustReleased, StillPressed, StillReleased};

struct Button {
	uint id{ 0 };
	int state{ ButtonState::StillReleased };
	int State() { return state; }
};
struct Analog {
	float x{ 0.0 };
	float y{ 0.0 };
};

struct Gamepad {
	int id;
	XINPUT_STATE state;
	XINPUT_STATE prevState;
public:
	Button A;
	Button B;
	Button X;
	Button Y;
	Button START;
	Button BACK;
	Button LT;
	Button RT;
	Button LS;
	Button RS;
	Button UP;
	Button DOWN;
	Button LEFT;
	Button RIGHT;
	Analog aLT;
	Analog aRT;
	Analog aLS;
	Analog aRS;
};

class Controller : public StoppableTask {
private:
	vector<Gamepad> gamepads;
	Gamepad currentGamepad;
	XInputGetStateEx_t XInputGetStateEx{ nullptr };
	HMODULE xInputLib{ nullptr };
private:
	bool ExtractHiddenInputStateFunction() {
		auto hiddenFunction = reinterpret_cast<LPCSTR>(100);
		xInputLib = LoadLibraryEx(L"xinput1_4.dll", 0, 0);
		if (xInputLib) {
			XInputGetStateEx = reinterpret_cast<XInputGetStateEx_t>(GetProcAddress(xInputLib, hiddenFunction));
			if (XInputGetStateEx)
			{
				printf("ExtractInputStateFunction: Successfully extracted hidden GetState function\n");
				return true;
			}
			else {
				XInputGetStateEx = reinterpret_cast<XInputGetStateEx_t>(GetProcAddress(xInputLib, "XInputGetState"));
				if (XInputGetStateEx) {
					printf("ExtractInputStateFunction: Could not extract hidden GetState function, fallback to regular one\n");
					return true;
				}
			}
			return false;
		}
		return false;
	}
public:
	void ConfigureDeadzone() {
		
	}
	void ReadControllerState() {
		if (gamepads.empty()) {
			for (auto i = 0; i < XUSER_MAX_COUNT; i++) {
				XINPUT_STATE state{};
				if (XInputGetStateEx(i, &state) == ERROR_SUCCESS) {
					gamepads.push_back({ i, state });
				}
				else {
					printf("Controller is not connected		\r");
				}
			}
			if (!gamepads.empty())
				currentGamepad = gamepads.front();
		}
		else {
			XINPUT_STATE state{};
			if (XInputGetStateEx(currentGamepad.id, &state) == ERROR_SUCCESS) {
				printf("Controller[%i] active\r", currentGamepad.id);
				if (state.dwPacketNumber != currentGamepad.state.dwPacketNumber) {
					printf("Controller[%i] state: changed\r", currentGamepad.id);
					currentGamepad.state = state;
				}
				else {
					printf("Controller[%i] state: idle    \r", currentGamepad.id);
				}
			}
			else {
				gamepads.clear();
				ReadControllerState();
				Tools::Instance()->SleepFor(4000);
			}
		}
	}
	void Init(){
		if (ExtractHiddenInputStateFunction()) {
			ReadControllerState();
		}
	};
	~Controller() {
		if (xInputLib) {
			FreeLibrary(xInputLib);
		}
	}
	Gamepad& Gamepad() { return currentGamepad; }
	bool IsPressed(uint button) {
		return (Gamepad().state.Gamepad.wButtons & button) != 0;
	}
	bool WasPressed(uint button) {
		return (Gamepad().prevState.Gamepad.wButtons & button) != 0;
	}
};

class Devices
{
public:
	Display m_display;
	Keyboard m_kbd;
	Controller m_pad;
public:
	void Print() {
		m_display.Print();
		//m_pad.Print();
	};
};

