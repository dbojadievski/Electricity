#include "Console.h"
#include "ArrayUtils.h"
#include "StringUtils.h"

#include <malloc.h>
#include <list>
#include <string>

CORE_BOOLEAN 
Console::VRegisterCommand(ConsoleCommand * pCommand) 
{
	assert(pCommand);
	assert(!pCommand->m_pStrName.empty());

	CORE_BOOLEAN wasCommandRegistered = false;

	if (!pCommand->m_pStrName.empty())
	{
		auto it = this->m_Commands.find(pCommand->m_pStrName);
		if (it == this->m_Commands.end())
		{
			auto insertionResult = this->m_Commands.insert_or_assign(pCommand->m_pStrName, pCommand);
			wasCommandRegistered = insertionResult.second;
		}
		else 
			assert(false);
	}

	return wasCommandRegistered;
}

CORE_BOOLEAN 
Console::VUnregisterCommand(CORE_STRING pStrCommandText) 
{
	assert(pStrCommandText);
	
	CORE_BOOLEAN anyErased = false;
	
	if (pStrCommandText)
	{
		size_t numErased = this->m_Commands.erase(pStrCommandText);
		anyErased = numErased;
	}

	return anyErased;
}


CORE_BOOLEAN 
Console::ParseCommandParameters(ConsoleCommand * pCommandDescriptor, list<string *> * pTokens, OUT_PARAM ConsoleCommandParameterList * pRetVal, size_t paramIdx)
{
	assert(pCommandDescriptor);
	assert(pTokens);
	assert(pRetVal);

	CORE_BOOLEAN isCommandValid = false;

	if (pCommandDescriptor && pTokens && pRetVal)
	{
		size_t numPotentialParams = (pTokens->size() - paramIdx);

		isCommandValid = (numPotentialParams == (pCommandDescriptor->m_pParameters->size()));
		if (isCommandValid)
		{
			std::list<string *>::iterator it = pTokens->begin();
			std::advance(it, paramIdx);
			if (it != pTokens->end())
			{

				int meaningOfLife = 42;
				meaningOfLife++;
				size_t currParamIdx = 0;
				while (it != pTokens->end())
				{
					string * pParamText = (*it);
					assert(pParamText);
					
					ConsoleCommandParameterBase * pParamDescriptor = (*(pCommandDescriptor->m_pParameters))[currParamIdx];
					assert(pParamDescriptor);

					CORE_DWORD asDword = -1;
					CORE_REAL asReal = -1;
					CORE_BOOLEAN asBoolean = false;
					ConsoleCommandParameterBase * pParam = NULL;
					switch (pParamDescriptor->m_Type)
					{
						case EConsoleCommandParameterType::PARAM_DWORD32:
							asDword = stoi(*pParamText);
							pParam = (ConsoleCommandParameterBase *) new ConsoleCommandParameter<CORE_DWORD>(pParamDescriptor->m_pStrName, pParamDescriptor->m_Type, asDword);
							break;
						case EConsoleCommandParameterType::REAL32:
							asReal = stof(*pParamText);
							pParam = (ConsoleCommandParameterBase *) new ConsoleCommandParameter<CORE_REAL>(pParamDescriptor->m_pStrName, pParamDescriptor->m_Type, asReal);
							break;
						case EConsoleCommandParameterType::PARAM_BOOLEAN:
							if (pParamText->compare("true"))
								pParam = (ConsoleCommandParameterBase *) new ConsoleCommandParameter<CORE_BOOLEAN>(pParamDescriptor->m_pStrName, pParamDescriptor->m_Type, true);
							else if (pParamText->compare("false"))
								pParam = (ConsoleCommandParameterBase *) new ConsoleCommandParameter<CORE_BOOLEAN>(pParamDescriptor->m_pStrName, pParamDescriptor->m_Type, false);
							else
								assert(false);
							break;
						case EConsoleCommandParameterType::STRING:
							pParam = (ConsoleCommandParameterBase *) new ConsoleCommandParameter<string>(pParamDescriptor->m_pStrName, pParamDescriptor->m_Type, *pParamText);
							break;
					}
					pRetVal->push_back(pParam);
				}
			}
			else if (numPotentialParams != 0)
				isCommandValid = false;
		}
	}

	return isCommandValid;
}

void 
Console::VParseCommand(CORE_STRING pStrCmdText)
{
	assert(pStrCmdText);

	CORE_BOOLEAN isCommandValid = false;
	if (pStrCmdText)
	{
		list<string *> * pTokens = tokenize(pStrCmdText);

		size_t numTokens = 0;
		if (pTokens && (numTokens = pTokens->size()))
		{
			string * pCmdText = pTokens->front();
			assert(pCmdText);
			auto it = this->m_Commands.find(pCmdText->c_str());
			assert(it != this->m_Commands.end());
			if (it != this->m_Commands.end())
			{
				ConsoleCommand * pCommand = (*it).second;
				assert(pCommand);

				ConsoleCommandParameterList * pParams = new ConsoleCommandParameterList();
				isCommandValid = ParseCommandParameters(pCommand, pTokens, pParams);
				if (isCommandValid)
				{
					/*TODO(Dino): Queue command! The command queue simply needs to be a list of key-value pairs, between the */
					std::pair<ConsoleCommand *, ConsoleCommandParameterList *> pair = std::make_pair(pCommand, pParams);
					this->m_Queue.push(pair);
				}
			}
		}
	}
}

const CVar * const 
Console::VGetCVar(CORE_STRING pStrName) const
{
	assert(pStrName);

	CVar * pRetVal = NULL;

	if (pStrName)
	{
		auto iter = this->m_CVars.find(pStrName);
		if (iter != this->m_CVars.end())
		{
			pRetVal = iter->second;
		}
	}

	return pRetVal;
}

CORE_BOOLEAN 
Console::VSetCVar(const CVar * const pCVar)
{
	assert(pCVar);
	assert(pCVar->m_pStrName);

	CORE_BOOLEAN isNewCVar = false;

	if (pCVar && pCVar->m_pStrName)
	{
		auto result = this->m_CVars.insert_or_assign(pCVar->m_pStrName, (CVar *)pCVar);
		isNewCVar = result.second;
	}

	return isNewCVar;
}

void
Console::Init()
{

}


void
Console::Update(CORE_REAL dT)
{
	size_t numCommandsQueued = this->m_Queue.size();

	if (!numCommandsQueued)
		goto end;

	while (this->m_Queue.size());
	{
		ConsoleCommandQueueEntry entry = this->m_Queue.front();
		
		ConsoleCommand * pCommand = entry.first;
		ConsoleCommandParameterList * pParams = entry.second;
		
		CommandHandlerDelegate * pCommandHandler = pCommand->m_pHandler;
		CommandHandlerDelegate asDelegate = *pCommandHandler;
		(*pCommandHandler)(pParams);
	
		this->m_Queue.pop();
	}

end:
	return;
}

void
Console::ShutDown()
{

}