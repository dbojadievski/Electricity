#pragma once
#include "CoreTypes.h"
#include "GameObject.h"
#include "IComponent.h"
#include "Console.h"
#include "EventManager.h"
#include "DirectX11Renderer.h"
#include "InputBase.h"
#include "TimeUtils.h"

#include <assert.h>
#include <vector>
class CoreEngine
{
private:
	CORE_BOOLEAN m_IsRunning;
	std::vector<IEngineSystem *> m_Systems;
	
	CORE_ULONG m_TickCount;
	CORE_DOUBLE m_totalTimeRunning;

	const IConsole * m_pConsole;
	IEventManager * m_pEventManager;
	InputBase * m_pInput;
	Timer * m_pTimer;
public:
	CoreEngine();

	void Start();
	void Stop();
#pragma region  Basic lifecycle events.

	void Init();
	void Update();
	void ShutDown();
#pragma endregion
	
	IConsole * GetConsole();
	IEventManager * GetEventManager();
};