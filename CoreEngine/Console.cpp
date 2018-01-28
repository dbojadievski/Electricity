#include "Console.h"
#include "ArrayUtils.h"
#include "StringUtils.h"

#include <malloc.h>
#include <list>
#include <string>

#include "FastDelegate.h"

using namespace fastdelegate;

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
Console::ParseCommandParameters(ConsoleCommand * pCommandDescriptor, list<string *> * pTokens, ConsoleCommandParameterList * pRetVal, size_t paramIdx)
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
					CORE_STRING pStrAsString = NULL;

					ConsoleCommandParameterBase * pParam = NULL;
					switch (pParamDescriptor->m_Type)
					{
						case EConsoleCommandParameterType::PARAM_DWORD32:
							asDword = stoi(*pParamText);
							pParam = (ConsoleCommandParameterBase *) new ConsoleCommandParameter<CORE_DWORD>(pParamDescriptor->m_pStrName, pParamDescriptor->m_Type, asDword);
							break;
						case EConsoleCommandParameterType::PARAM_REAL32:
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
						case EConsoleCommandParameterType::PARAM_STRING:
							/*NOTE(Dino): We're copying the text value of the token because the caller needs to release the token objects themselves. */
							string * pTokenValue(pParamText);
							pParam = (ConsoleCommandParameterBase *) new ConsoleCommandParameter<string>(pParamDescriptor->m_pStrName, pParamDescriptor->m_Type, *pTokenValue);
							break;
					}
					pRetVal->push_back(pParam);
					it++;
					currParamIdx++;
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

			auto tokIter = pTokens->begin();
			while (tokIter != pTokens->end())
			{
				string * pStr = *tokIter;
				delete pStr;
				tokIter++;
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
Console::RegisterAllCommands()
{
	{
		CORE_BOOLEAN wasRegistered = false;

		CORE_STRING pStrCommandText = "entity_create";
		ConsoleCommandParameterDescriptor * pParamEntityName = new ConsoleCommandParameterDescriptor("entity_name", EConsoleCommandParameterType::PARAM_STRING);

		ConsoleCommandParameterList * pParamsList = new ConsoleCommandParameterList();
		pParamsList->push_back(pParamEntityName);

		CommandHandlerDelegate  commandDelegate = MakeDelegate(this, &Console::OnEntityCreateHandler);
		CommandHandlerDelegate * pCommandDelegate = new CommandHandlerDelegate(commandDelegate);
		
		ConsoleCommand * pCommand = new ConsoleCommand(pStrCommandText, pParamsList, pCommandDelegate);
		wasRegistered = this->VRegisterCommand(pCommand);
		assert(wasRegistered);

		//NOTE(Dino): Use command as: 
		//this->VParseCommand("entity_create ananas");
	}

	{
		CORE_BOOLEAN wasRegistered = false;

		CORE_STRING pStrCommandText = "entity_destroy";
		ConsoleCommandParameterDescriptor * pParamEntityName = new ConsoleCommandParameterDescriptor("entity_name", EConsoleCommandParameterType::PARAM_STRING);

		ConsoleCommandParameterList * pParamsList = new ConsoleCommandParameterList();
		pParamsList->push_back(pParamEntityName);

		CommandHandlerDelegate  commandDelegate = MakeDelegate(this, &Console::OnEntityDestroyHandler);
		CommandHandlerDelegate * pCommandDelegate = new CommandHandlerDelegate(commandDelegate);

		ConsoleCommand * pCommand = new ConsoleCommand(pStrCommandText, pParamsList, pCommandDelegate);
		wasRegistered = this->VRegisterCommand(pCommand);
		assert(wasRegistered);

		//NOTE(Dino): Use command as: 
		//this->VParseCommand("entity_destroy ananas");
	}

	{
		CORE_BOOLEAN wasRegistered = false;

		CORE_STRING pStrCommandText = "entity_link";
		ConsoleCommandParameterDescriptor * pParamParentName = new ConsoleCommandParameterDescriptor("parent_name", EConsoleCommandParameterType::PARAM_STRING);
		ConsoleCommandParameterDescriptor * pParamChildName = new ConsoleCommandParameterDescriptor("child_name", EConsoleCommandParameterType::PARAM_STRING);

		ConsoleCommandParameterList * pParamsList = new ConsoleCommandParameterList();
		pParamsList->push_back(pParamParentName);
		pParamsList->push_back(pParamChildName);


		CommandHandlerDelegate  commandDelegate = MakeDelegate(this, &Console::OnEntitiesLinkHandler);
		CommandHandlerDelegate * pCommandDelegate = new CommandHandlerDelegate(commandDelegate);

		ConsoleCommand * pCommand = new ConsoleCommand(pStrCommandText, pParamsList, pCommandDelegate);
		wasRegistered = this->VRegisterCommand(pCommand);
		assert(wasRegistered);

		//NOTE(Dino): Use command as: 
		//this->VParseCommand("entity_link my_parent_entity my_child_entity");
		
	}

	{
		CORE_BOOLEAN wasRegistered = false;

		CORE_STRING pStrCommandText = "entity_unlink";
		ConsoleCommandParameterDescriptor * pParentEntityName = new ConsoleCommandParameterDescriptor("entity_name", EConsoleCommandParameterType::PARAM_STRING);

		ConsoleCommandParameterList * pParamsList = new ConsoleCommandParameterList();
		pParamsList->push_back(pParentEntityName);

		CommandHandlerDelegate  commandDelegate = MakeDelegate(this, &Console::OnEntitiesUnLinkHandler);
		CommandHandlerDelegate * pCommandDelegate = new CommandHandlerDelegate(commandDelegate);

		ConsoleCommand * pCommand = new ConsoleCommand(pStrCommandText, pParamsList, pCommandDelegate);
		wasRegistered = this->VRegisterCommand(pCommand);
		assert(wasRegistered);

		//NOTE(Dino): Use command as: 
		//this->VParseCommand("entity_unlink my_child_entity");
	}

#pragma region Unit testing. If the debug build does not assert, then all is well.
	{
#if DEBUG
		this->VParseCommand("entity_create parent");
		this->VParseCommand("entity_create child");
		this->VParseCommand("entity_link parent child");
		this->VParseCommand("entity_unlink child");
		this->VParseCommand("entity_destroy parent");
		this->VParseCommand("entity_destroy child");
#endif
	}
#pragma endregion
}

void
Console::Init()
{
	this->RegisterAllCommands();
}

void
Console::Update(CORE_DOUBLE dT)
{
	size_t numCommandsQueued = this->m_Queue.size();

	if (!numCommandsQueued)
		goto end;

	size_t queueSize = this->m_Queue.size();
	while (this->m_Queue.size() > 0 )
	{
		ConsoleCommandQueueEntry entry = this->m_Queue.front();
		
		ConsoleCommand * pCommand = entry.first;
		ConsoleCommandParameterList * pParams = entry.second;
		
		CommandHandlerDelegate * pCommandHandler = pCommand->m_pHandler;
		CommandHandlerDelegate asDelegate = *pCommandHandler;
		(*pCommandHandler)(pParams);
	
		this->CleanUpCommand(&entry);
		this->m_Queue.pop();
	}

end:
	return;
}

void
Console::CleanUpCommand(ConsoleCommandQueueEntry * pEntry)
{
	assert(pEntry);
	ConsoleCommandParameterList * pParams = pEntry->second;
	if (pParams && pParams->size())
	{
		size_t numParams = pParams->size();
		for (size_t currParamIdx = 0; currParamIdx < numParams; currParamIdx++)
		{
			ConsoleCommandParameterBase * pParam = pParams->at(currParamIdx);
			delete pParam;
		}

		pParams->clear();
		delete pParams;
	}
}
void
Console::ShutDown()
{
	/*Clears the commands themselves. */
	{
		auto commandIt = this->m_Commands.begin();
		while (commandIt != this->m_Commands.end())
		{
			ConsoleCommand * pCommand = commandIt->second;
			assert(pCommand);
			free(pCommand);
			commandIt++;
		}

		this->m_Commands.clear();
	}

	/*Clears the command queue. */
	{
		while (this->m_Queue.size())
		{
			ConsoleCommandQueueEntry item = this->m_Queue.front();
			free(item.second);
			this->m_Queue.pop();
		}
	}

	/*Clears the CVars. */
	{
		auto cvarIt = this->m_CVars.begin();
		while (cvarIt != this->m_CVars.end())
		{
			CORE_STRING pStrCvarName = cvarIt->first;
			CVar * pCvar = cvarIt->second;
			free(pCvar);
			cvarIt++;

		}
		this->m_CVars.clear();
	}
}