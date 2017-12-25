#pragma once
#include "CoreTypes.h"
#include "GameObject.h"
#include "IComponent.h"
#include "Console.h"
#include <assert.h>
#include "DirectX11Renderer.h"
#include <vector>

class CoreEngine
{
private:
	std::vector<IEngineSystem *> m_Systems;
	
	CORE_ULONG m_TickCount;
	CORE_REAL m_totalTimeRunning;

	const IConsole * m_pConsole;
public:
	CoreEngine()
	{
		this->m_TickCount = 0;
		this->m_totalTimeRunning = 0;
	}

#pragma region  Basic lifecycle events.
	void Init()
	{
		IConsole * pConsole = new Console();
		IEngineSystem * pConsoleSystem = (IEngineSystem *)pConsole;
		DirectX11Renderer * pRenderer = new DirectX11Renderer();
		IEngineSystem * pRendererSystem = (IEngineSystem *)pRenderer;

		this->m_Systems.push_back(pConsoleSystem);
		this->m_pConsole = pConsole;

		this->m_Systems.push_back(pRendererSystem);

		size_t numSystems = this->m_Systems.size();
		for (size_t currSystemIdx = 0; currSystemIdx < numSystems; currSystemIdx++)
		{
			IEngineSystem * pSystem = this->m_Systems[currSystemIdx];
			pSystem->Init();
		}
	}

	void Update()
	{
		size_t numSystems = this->m_Systems.size();
		for (size_t currSystemIdx = 0; currSystemIdx < numSystems; currSystemIdx++)
		{
			IEngineSystem * pSystem = this->m_Systems[currSystemIdx];
			pSystem->Update(this->m_totalTimeRunning);
		}
	}

	void ShutDown()
	{
		size_t numSystems = this->m_Systems.size();
		for (size_t currSystemIdx = 0; currSystemIdx < numSystems; currSystemIdx++)
		{
			IEngineSystem * pSystem = this->m_Systems[currSystemIdx];
			pSystem->ShutDown();
			delete(pSystem);
		}
	}
#pragma endregion


	IConsole  * GetConsole()
	{
		return (IConsole *) this->m_pConsole;
	}
};