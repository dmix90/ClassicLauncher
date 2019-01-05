#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <stdexcept>
#include <chrono>
#include <thread>
#include <future>
#include <algorithm>
#include <fstream>

#include <Windows.h>
#include <tlhelp32.h>
#include <Shobjidl.h>
#include <atlbase.h>
#include <Xinput.h>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.ApplicationModel.h>
#include <winrt/Windows.Management.h>
#include <winrt/Windows.Management.Deployment.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.Storage.h>
#include <winrt/Windows.Data.Xml.Dom.h>

//#include <winrt/Windows.Gaming.Input.h>

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::ApplicationModel;
using namespace Windows::Management::Deployment;
using namespace Windows::Storage;
using namespace Windows::Data::Xml::Dom;
//using namespace Windows::Gaming::Input;

using namespace std::this_thread;
using namespace std::chrono_literals;
using std::chrono::system_clock;

using std::string;
using std::wstring;
using std::wstring_view;
using std::vector;
using std::unique_ptr;
using std::make_unique;
using std::exception;
using std::map;
using std::pair;
using std::stoi;
using std::stof;
using std::make_pair;
using std::max_element;
using std::min_element;
using std::tuple;
using std::make_tuple;
using std::get;
using std::array;
using std::thread;
using std::promise;
using std::future;
using std::move;

using std::wofstream;
using std::wifstream;

using ulong = unsigned long;
using uint	= unsigned short;