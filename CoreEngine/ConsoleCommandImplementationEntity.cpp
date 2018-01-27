#include "Console.h"
#include "CoreHeaders.h"
#include "CoreEngine.h"
#include <string>

using namespace std;

void
Console::OnEntityCreateHandler(ConsoleCommandParameterList * pParams) const
{
	CORE_ERROR errCode = ERR_OK;

	assert(pParams);
	assert(pParams->size());

	ConsoleCommandParameterBase * pName = pParams->at(0);
	assert(pName);
	assert(pName->m_Type == PARAM_STRING && pName->m_pStrName == "entity_name");
	if (pName && pName->m_Type == PARAM_STRING && pName->m_pStrName == "entity_name")
	{
		ConsoleCommandParameter<string> * pNameCast = (ConsoleCommandParameter<string> *) pParams->at(0);
		EntitySystem * pEntitySystem = g_Engine.GetEntitySystem();
		assert(pEntitySystem);

		string * pStrEntityName = new string(pNameCast->GetValue());
		Entity * pEntity = new Entity(pStrEntityName);
		errCode = pEntitySystem->RegisterEntity(pEntity);
		assert(errCode == ERR_OK);
	}
}

void
Console::OnEntityDestroyHandler(ConsoleCommandParameterList * pParams) const
{
	CORE_ERROR errCode = ERR_OK;

	assert(pParams);
	assert(pParams->size());

	ConsoleCommandParameterBase * pName = pParams->at(0);
	assert(pName);
	assert(pName->m_Type == PARAM_STRING && pName->m_pStrName == "entity_name");
	if (pName && pName->m_Type == PARAM_STRING && pName->m_pStrName == "entity_name")
	{
		ConsoleCommandParameter<string> * pNameCast = (ConsoleCommandParameter<string> *) pParams->at(0);
		EntitySystem * pEntitySystem = g_Engine.GetEntitySystem();
		assert(pEntitySystem);

		string entityName = pNameCast->GetValue();
		Entity * pEntity = NULL;	
		errCode = pEntitySystem->GetEntityByTag(&entityName, &pEntity);
		assert(errCode == ERR_OK);
		if (errCode == ERR_OK)
		{
			errCode = pEntitySystem->UnRegisterEntity(pEntity->GetIdentifier());
			assert(errCode == ERR_OK);
			if (errCode == ERR_OK)
				delete pEntity;
		}
	}
}
