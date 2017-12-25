#include "ConsoleCommand.h"

ConsoleCommand::ConsoleCommand(CORE_STRING pStrCommandText, ConsoleCommandParameterList * pParams, CommandHandlerDelegate  * pHandlerDelegate)
{
	this->m_pStrName = string(pStrCommandText);
	this->m_pParameters = pParams;
	this->m_pHandler = pHandlerDelegate;
}