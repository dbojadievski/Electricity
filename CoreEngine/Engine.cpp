#include "Engine.h"

CoreEngine::CoreEngine()
{
	this->m_IsRunning = false;
	this->m_TickCount = 0;
	this->m_totalTimeRunning = 0;
}

void
CoreEngine::Init()
{
	this->m_pEventManager = new EventManager("EVENT_MANAGER_MAIN", true);
	
	IConsole * pConsole = new Console();
	IEngineSystem * pConsoleSystem = (IEngineSystem *)pConsole;
	this->m_pConsole = pConsole;
	this->m_Systems.push_back(pConsoleSystem);
	
	DirectX11Renderer * pRenderer = new DirectX11Renderer(this->m_pEventManager);
	IEngineSystem * pRendererSystem = (IEngineSystem *)pRenderer;
	this->m_Systems.push_back(pRendererSystem);
	
	this->m_pInput = new InputBase(this->m_pEventManager);
	this->m_Systems.push_back(this->m_pInput);

	this->m_pTimer = new Timer();


	size_t numSystems = this->m_Systems.size();
	for (size_t currSystemIdx = 0; currSystemIdx < numSystems; currSystemIdx++)
	{
		IEngineSystem * pSystem = this->m_Systems[currSystemIdx];
		pSystem->Init();
	}

	this->m_pTimer->Init();
	CORE_DOUBLE timeSinceUpdate = this->m_pTimer->GetTime();
}

void
CoreEngine::Update()
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

void
CoreEngine::ShutDown()
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

void
CoreEngine::Start()
{
	this->m_IsRunning = true;
}
void 
CoreEngine::Stop()
{
	this->m_IsRunning = false;
}

IConsole *
CoreEngine::GetConsole()
{
	return (IConsole *)this->m_pConsole;
}

IEventManager *
CoreEngine::GetEventManager()
{
	return this->m_pEventManager;
}