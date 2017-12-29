#include <stdio.h>

#include "FastDelegate.h"
#include "FastDelegateBind.h"
#include "GameObject.h"
#include "IEventData.h"
#include "IEventManager.h"
#include "EventManager.h"

#include "ConsoleCommandParameter.h"
#include "FastDelegate.h"
#include "Console.h"

#include "AssetLoader.h"

#include <ostream>

#include <malloc.h>
#include <io.h>
#include <fcntl.h>

#include "Engine.h"
#include "ResolutionChangedEventData.h"

#include <iostream>
#include <filesystem>

#include <Windows.h>
#include <windowsx.h>

#include "DirectX11Renderer.h"

using namespace fastdelegate;

CoreEngine g_Engine;
HWND g_Window;
HINSTANCE g_hInstance;

EventManager manager("EVENT_MANAGER_MAIN", true);
class TestEventData : public EventDataBase
{
public:

	virtual IEventData * VCopy(void) const
	{
		return (IEventData *) this;
	}

	virtual const char * VGetName() const
	{
		return "TEST EVENT DATA";
	}

	float EventDataBase::VGetTimeStamp(void) const
	{
		return this->m_timeStamp;
	}
	virtual const EventType& VGetEventType(void) const
	{
		return EVENT_TYPE_TEST;
	}

	virtual ~TestEventData()
	{
	}

	virtual void EventDataBase::VSerialize(std::ostream &out) const
	{
		//TODO something.
	}
};
class TestSystem
{
public:
	void TestDelegateHandler(IEventData * pEvent)
	{
		printf("%s\n", pEvent->VGetName());
	}

	void TestEventSystem()
	{
		EventListenerDelegate eDelegate = MakeDelegate(this, &TestSystem::TestDelegateHandler);
		manager.VAddListener(eDelegate, EVENT_TYPE_TEST);

		TestEventData * data = new TestEventData();
		EventType type = data->VGetEventType();
		manager.VTriggerEvent(data);

		manager.VQueueEvent(data);
		manager.VTickUpdate();

		manager.VRemoveListener(eDelegate, EVENT_TYPE_TEST);
		manager.VTriggerEvent(data);
	}

	void TestCommandHandler(ConsoleCommandParameterList * pParams)
	{
		printf("Hello from the command handler!\n");
	}
};

void TestHandler(ConsoleCommandParameterList * pParams)
{

}

void TestConsole(IConsole * pConsole, TestSystem * pSystem)
{
	ConsoleCommandParameter<CORE_DWORD> meaningOfLife("meaning_of_life", EConsoleCommandParameterType::PARAM_DWORD32, 42);
	pConsole->VSetCVar((const CVar *)&meaningOfLife);
	const CVar * const pCVarMeaningOfLife = pConsole->VGetCVar("meaning_of_life");
	CVarDWord * pModifiablePtr = (CVarDWord *)pCVarMeaningOfLife;

	ConsoleCommandParameterList * pParams = (ConsoleCommandParameterList *)malloc(sizeof(ConsoleCommandParameterList));
	CommandHandlerDelegate testDelegate = MakeDelegate(pSystem, &TestSystem::TestCommandHandler);
	ConsoleCommand testCommand("test_command", pParams, &testDelegate);
	CORE_BOOLEAN isRegistered = pConsole->VRegisterCommand(&testCommand);
	assert(isRegistered);

	pConsole->VParseCommand("test_command");
}

void TestLoadAsset()
{
	CORE_BOOLEAN wasAssetLoaded = false;

	string assetPath("Assets\\Models\\Dwarf\\dwarf.obj");
	FILE * pFile = fopen(assetPath.c_str(), "r");
	if (pFile)
	{
		printf("File read.\n");
		fclose(pFile);
		wasAssetLoaded = ImportAsset(&assetPath);
		assert(wasAssetLoaded);
	}
	else
	{
		printf("File not read.");
	}

}

LRESULT CALLBACK WindowProc(HWND hWnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam);

// the entry point for any Windows program
int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{

	///*NOTE(Dino):Set up the console, so we can do debug logging. */
	AllocConsole();

	HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
	int hCrt = _open_osfhandle((long)handle_out, _O_TEXT);
	FILE* hf_out = _fdopen(hCrt, "w");
	setvbuf(hf_out, NULL, _IONBF, 1);
	*stdout = *hf_out;

	HANDLE handle_in = GetStdHandle(STD_INPUT_HANDLE);
	hCrt = _open_osfhandle((long)handle_in, _O_TEXT);
	FILE* hf_in = _fdopen(hCrt, "r");
	setvbuf(hf_in, NULL, _IONBF, 128);
	*stdin = *hf_in;

	// the handle for the window, filled by a function
	HWND hWnd;
	// this struct holds information for the window class
	WNDCLASSEX wc;

	// clear out the window class for use
	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	// fill in the struct with the needed information
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszClassName = "WindowClass1";

	// register the window class
	RegisterClassEx(&wc);

	HMONITOR activeMonitor = MonitorFromWindow(GetActiveWindow(), MONITOR_DEFAULTTONEAREST);
	MONITORINFOEX monitorInfo;
	ZeroMemory(&monitorInfo, sizeof(monitorInfo));
	monitorInfo.cbSize = sizeof(MONITORINFOEX);
	GetMonitorInfo(activeMonitor, &monitorInfo);

	// create the window and use the result as the handle
	hWnd = CreateWindowEx(NULL,
		"WindowClass1",    // name of the window class
		"Electricity",   // title of the window
		WS_OVERLAPPEDWINDOW,    // window style
		0,    // x-position of the window
		0,    // y-position of the window
		monitorInfo.rcWork.right,    // width of the window
		monitorInfo.rcWork.bottom,    // height of the window
		NULL,    // we have no parent window, NULL
		NULL,    // we aren't using menus, NULL
		hInstance,    // application handle
		NULL);    // used with multiple windows, NULL

				  // display the window on the screen
	g_Window = hWnd;
	g_hInstance = hInstance;
	ShowWindow(hWnd, nCmdShow);

	g_Engine.Init();

	ConsoleCommandParameter<CORE_DWORD> paramDisplayWidth("display_resolution_width", EConsoleCommandParameterType::PARAM_DWORD32, monitorInfo.rcWork.right);
	g_Engine.GetConsole()->VSetCVar((const CVar *) &paramDisplayWidth);

	ConsoleCommandParameter<CORE_DWORD> paramDisplayHeight("display_resolution_height", EConsoleCommandParameterType::PARAM_DWORD32, monitorInfo.rcWork.bottom);
	g_Engine.GetConsole()->VSetCVar((const CVar *) &paramDisplayHeight);

	ResolutionChangedEventData * pResChangedData = new ResolutionChangedEventData();
	pResChangedData->m_Width = monitorInfo.rcWork.right;
	pResChangedData->m_Height = monitorInfo.rcWork.bottom;
	g_Engine.GetEventManager()->VTriggerEvent(pResChangedData);

	g_Engine.Start();

	// this struct holds Windows event messages
	MSG msg;


	while (true)
	{
		g_Engine.Update();
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT || msg.message == WM_CLOSE ||
				msg.message == WM_DESTROY)
			{
				g_Engine.ShutDown();
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
	}

	// return this part of the WM_QUIT message to Windows
	return msg.wParam;
}

// this is the main message handler for the program
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// sort through and find what code to run for the message given
	switch (message)
	{
		// this message is read when the window is closed
	case WM_DESTROY:
	{
		// close the application entirely
		PostQuitMessage(0);
		return 0;
	} break;
	}

	// Handle any messages the switch statement didn't
	return DefWindowProc(hWnd, message, wParam, lParam);
}