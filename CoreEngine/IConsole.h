#pragma once
#include "CoreTypes.h"
#include "ConsoleCommandParameter.h"
#include "ConsoleCommand.h"
#include "IEngineSystem.h"

#include <vector>
class IConsole : IEngineSystem
{
public:
	virtual CORE_BOOLEAN VRegisterCommand(ConsoleCommand * pCommand) = 0;
	virtual CORE_BOOLEAN VUnregisterCommand(CORE_STRING pStrCommandText) = 0;
	virtual void VParseCommand(const char * pStrCmdText) = 0;
	//virtual void VQueueCommand(ConsoleCommandQueueEntry * pEntry) = 0;

	virtual const CVar * const VGetCVar(CORE_STRING pStrName) const = 0;
	virtual CORE_BOOLEAN VSetCVar(const CVar * const pCVar) = 0;
};