#pragma once
#include "CoreTypes.h"
#include "IConsole.h"
#include "ConsoleCommand.h"

#include "FastDelegate.h"

#include <list>

using std::list;
using std::string;
using std::pair;
using std::map;
using std::queue;

typedef std::map<CORE_STRING, CVar *> CVarMap;
typedef std::map<string, ConsoleCommand *> ConsoleCommandMap;
typedef pair<ConsoleCommand *, ConsoleCommandParameterList *> ConsoleCommandQueueEntry;
typedef std::queue<ConsoleCommandQueueEntry> ConsoleCommandQueue;

class Console : public IConsole
{
private:
	CVarMap m_CVars;
	ConsoleCommandMap m_Commands;
	ConsoleCommandQueue m_Queue;

	static CORE_BOOLEAN ParseCommandParameters(ConsoleCommand * pCommandDescriptor, list<string *> * pTokens, ConsoleCommandParameterList * pRetVal, size_t paramIdx = 1);

#pragma region Command handlers.
	void OnEntityCreateHandler(ConsoleCommandParameterList * pParams) const;
	void OnEntityDestroyHandler(ConsoleCommandParameterList * pParams) const;
	void OnEntitiesLinkHandler(ConsoleCommandParameterList * pParams) const;
	void OnEntitiesUnLinkHandler(ConsoleCommandParameterList * pParams) const;
	void OnRenderableInstantiateHandler(ConsoleCommandParameterList * pParams) const;
	void OnSceneLoadHandler(ConsoleCommandParameterList * pParams) const;
	void OnEntityTranslateHandler (ConsoleCommandParameterList * pParams) const;
#pragma endregion
	void RegisterAllCommands();
public:
	virtual CORE_BOOLEAN IConsole::VRegisterCommand(ConsoleCommand * pCommand);
	virtual CORE_BOOLEAN IConsole::VUnregisterCommand(CORE_STRING pStrCommandText);
	virtual void IConsole::VParseCommand(const char * pStrCmdText);
	//virtual void IConsole::VQueueCommand(ConsoleCommandQueueEntry * pEntry);

	virtual const CVar * const IConsole::VGetCVar(CORE_STRING pStrName) const;
	virtual CORE_BOOLEAN IConsole::VSetCVar(const CVar * const pCVar);

	virtual void IEngineSystem::Init(void);
	void CleanUpCommand(ConsoleCommandQueueEntry * pEntry);
	virtual void IEngineSystem::Update(CORE_DOUBLE dT);
	virtual void IEngineSystem::ShutDown();
};