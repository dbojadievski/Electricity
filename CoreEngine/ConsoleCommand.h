#pragma once

#include "FastDelegate.h"
#include <vector>
#include "ConsoleCommandParameter.h"

typedef vector<ConsoleCommandParameterBase *> ConsoleCommandParameterList;
typedef fastdelegate::FastDelegate1<ConsoleCommandParameterList *> CommandHandlerDelegate;

class ConsoleCommand
{
	friend class Console;
private:
	string m_pStrName;
	ConsoleCommandParameterList *m_pParameters;
	CommandHandlerDelegate * m_pHandler;

public:
	ConsoleCommand(CORE_STRING pStrCommandText, ConsoleCommandParameterList * pParams, CommandHandlerDelegate * pDelegate);
};