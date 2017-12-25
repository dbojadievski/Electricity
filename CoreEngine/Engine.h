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
	CoreEngine()
	{
		this->m_IsRunning = false;
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
		this->m_pTimer = new Timer();

		this->m_Systems.push_back(pConsoleSystem);
		this->m_pConsole = pConsole;

		this->m_Systems.push_back(pRendererSystem);

		size_t numSystems = this->m_Systems.size();
		for (size_t currSystemIdx = 0; currSystemIdx < numSystems; currSystemIdx++)
		{
			IEngineSystem * pSystem = this->m_Systems[currSystemIdx];
			pSystem->Init();
		}

		this->m_pTimer->Init();
		CORE_DOUBLE timeSinceUpdate = this->m_pTimer->GetTime();
	}

	void Start()
	{
		this->m_IsRunning = true;
	}

	void Stop()
	{
		this->m_IsRunning = false;
	}

	void Update()
	{
		if (!this->m_IsRunning)
			goto end;

		size_t numSystems = this->m_Systems.size();
		for (size_t currSystemIdx = 0; currSystemIdx < numSystems; currSystemIdx++)
		{
			IEngineSystem * pSystem = this->m_Systems[currSystemIdx];
			pSystem->Update(this->m_totalTimeRunning);
			
			CORE_DOUBLE timeSinceUpdate = this->m_pTimer->GetTime();
			this->m_totalTimeRunning += timeSinceUpdate;
		
			this->m_TickCount++;
			printf("Finished tick %l at %d.", this->m_TickCount, this->m_totalTimeRunning);
		}

	end:
		return;
	}

	void ShutDown()
	{
		this->m_IsRunning = false;
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