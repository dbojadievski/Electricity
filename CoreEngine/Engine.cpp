#include "Engine.h"
#include "DirectX11Renderer.h"
CoreEngine::CoreEngine()
{
	this->m_IsRunning			= false;
	this->m_TickCount			= 0;
	this->m_totalTimeRunning	= 0;
}

void
CoreEngine::Init()
{
	this->m_pEventManager		= new EventManager("EVENT_MANAGER_MAIN", true);
	
	IConsole * pConsole				= new Console();
	IEngineSystem * pConsoleSystem	= (IEngineSystem *)pConsole;
	this->m_pConsole				= pConsole;
	this->m_Systems.push_back(pConsoleSystem);
	
	DirectX11Renderer * pRenderer	= new DirectX11Renderer(this->m_pEventManager);
	IEngineSystem * pRendererSystem = (IEngineSystem *)pRenderer;
	this->m_pRenderer				= (IRenderer *) pRendererSystem;
	
	this->m_pInput					= new InputBase(this->m_pEventManager);
	this->m_Systems.push_back(this->m_pInput);

	this->m_pEntitySystem			= new EntitySystem(this->m_pEventManager);
	this->m_pTimer					= new Timer();

	IAssetManager * pAssetManager = new AssetManager(this->m_pEventManager);
	this->m_pAssetManager			= pAssetManager;
	this->m_Systems.push_back((IEngineSystem *) pAssetManager);
	this->m_pAssetManager			= pAssetManager;
	this->m_Systems.push_back (pRendererSystem);

	size_t numSystems				= this->m_Systems.size();
	for (size_t currSystemIdx = 0; currSystemIdx < numSystems; currSystemIdx++)
	{
		IEngineSystem * pSystem		= this->m_Systems[currSystemIdx];
		pSystem->Init();
	}

	this->m_pTimer->Init();
	CORE_DOUBLE timeSinceUpdate		= this->m_pTimer->GetTime();
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
	}

	this->m_pEventManager->VTickUpdate();

	CORE_DOUBLE timeSinceUpdate = this->m_pTimer->GetTime();
	this->m_totalTimeRunning += timeSinceUpdate;

	this->m_TickCount++;
	printf("Finished tick %d at %f.", this->m_TickCount, this->m_totalTimeRunning);

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
CoreEngine::GetConsole() const
{
	return (IConsole *)this->m_pConsole;
}

IEventManager *
CoreEngine::GetEventManager() const
{
	return (IEventManager *) this->m_pEventManager;
}

EntitySystem *
CoreEngine::GetEntitySystem() const
{
	return (EntitySystem * ) this->m_pEntitySystem;
}

IAssetManager *
CoreEngine::GetAssetManager () const
{
    return (IAssetManager *)this->m_pAssetManager;
}

IRenderer *
CoreEngine::GetRenderSystem() const
{
	return (IRenderer *) this->m_pRenderer;
}

void
CoreEngine::RegisterCommands()
{
	{
		/*NOTE(Dino): Registers the entity_create command. */
		ConsoleCommandParameterList * pParams = new ConsoleCommandParameterList();
		//CommandHandlerDelegate commandDelegate = MakeDelegate(this->)
	}
}