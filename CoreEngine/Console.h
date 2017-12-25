#pragma once
#include "CoreTypes.h"
#include "IConsole.h"
#include "ConsoleCommand.h"

#include "FastDelegate.h"

#include <list>

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

	static CORE_BOOLEAN ParseCommandParameters(ConsoleCommand * pCommandDescriptor, list<string *> * pTokens, OUT_PARAM ConsoleCommandParameterList * pRetVal, size_t paramIdx = 1);

public:
	virtual CORE_BOOLEAN IConsole::VRegisterCommand(ConsoleCommand * pCommand);
	virtual CORE_BOOLEAN IConsole::VUnregisterCommand(CORE_STRING pStrCommandText);
	virtual void IConsole::VParseCommand(const char * pStrCmdText);

	virtual const CVar * const IConsole::VGetCVar(CORE_STRING pStrName) const;
	virtual CORE_BOOLEAN IConsole::VSetCVar(const CVar * const pCVar);

	virtual void IEngineSystem::Init(void);
	virtual void IEngineSystem::Update(CORE_DOUBLE dT);
	virtual void IEngineSystem::ShutDown();
};