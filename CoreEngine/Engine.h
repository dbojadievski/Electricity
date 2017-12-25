#pragma once
#include "CoreTypes.h"
#include "GameObject.h"
#include "IComponent.h"
#include "Console.h"
#include <assert.h>
#include "DirectX11Renderer.h"
#include <vector>
#include "TimeUtils.h"
class CoreEngine
{
private:
	CORE_BOOLEAN m_IsRunning;
	std::vector<IEngineSystem *> m_Systems;
	
	CORE_ULONG m_TickCount;
	CORE_REAL m_totalTimeRunning;

	const IConsole * m_pConsole;

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
};