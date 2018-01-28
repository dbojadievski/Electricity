#include "Console.h"
#include "CoreHeaders.h"
#include "CoreEngine.h"
#include <string>
#include "EntityEvents.h"
using namespace std;

void
Console::OnEntityCreateHandler(ConsoleCommandParameterList * pParams) const
{
	CORE_ERROR errCode = ERR_OK;

	assert(pParams);
	assert(pParams->size() == 1);

	ConsoleCommandParameterBase * pName = pParams->at(0);
	assert(pName);
	assert(pName->m_Type == PARAM_STRING && pName->m_pStrName == "entity_name");
	if (pName && pName->m_Type == PARAM_STRING && pName->m_pStrName == "entity_name")
	{
		ConsoleCommandParameter<string> * pNameCast = (ConsoleCommandParameter<string> *) pParams->at(0);
		EntitySystem * pEntitySystem = g_Engine.GetEntitySystem();
		assert(pEntitySystem);

		Entity * pEntity = NULL;
		string * pStrEntityName = new string(pNameCast->GetValue());
		errCode = pEntitySystem->GetEntityByTag(pStrEntityName, &pEntity);
		assert(errCode == ERR_OBJECT_NOT_FOUND);
		if (errCode == ERR_OBJECT_NOT_FOUND)
		{
			pEntity = new Entity(pStrEntityName);
			errCode = pEntitySystem->RegisterEntity(pEntity);
			assert(errCode == ERR_OK);

			if (errCode == ERR_OK)
			{
				IEventManager * pEventManager = g_Engine.GetEventManager();
				assert(pEventManager);
				EntityRegisteredEventData * pEventData = new EntityRegisteredEventData();
				pEventData->m_Identifier = pEntity->GetIdentifier();
				pEventManager->VQueueEvent(pEventData);
			}
		}
	}
}

void
Console::OnEntityDestroyHandler(ConsoleCommandParameterList * pParams) const
{
	CORE_ERROR errCode = ERR_OK;

	assert(pParams);
	assert(pParams->size() == 1);

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
			{
				IEventManager * pEventManager = g_Engine.GetEventManager();
				assert(pEventManager);	
				EntityUnRegisteredEventData * pEventData = new EntityUnRegisteredEventData();
				pEventData->m_Identifier = pEntity->GetIdentifier();
				pEventManager->VQueueEvent(pEventData);
				
				delete pEntity;
			}
		}
	}
}

void
Console::OnEntitiesLinkHandler(ConsoleCommandParameterList * pParams) const
{
	CORE_ERROR errCode = ERR_OK;

	assert(pParams);
	assert(pParams->size() == 2);

	if (pParams && pParams->size() == 2)
	{
		ConsoleCommandParameterBase * pParentName = pParams->at(0);
		assert(pParentName);
		assert(pParentName->m_Type == PARAM_STRING && pParentName->m_pStrName == "parent_name");

		ConsoleCommandParameterBase * pChildName = pParams->at(1);
		assert(pChildName);
		assert(pChildName->m_Type == PARAM_STRING && pChildName->m_pStrName == "child_name");

		if ((pParentName && pParentName->m_Type == PARAM_STRING && pParentName->m_pStrName == "parent_name") && (pChildName && pChildName->m_Type == PARAM_STRING && pChildName->m_pStrName == "child_name"))
		{
			ConsoleCommandParameter<string> * pParentNameCast = (ConsoleCommandParameter<string> *) pParentName;
			ConsoleCommandParameter<string> * pChildNameCast = (ConsoleCommandParameter<string> *) pChildName;
			
			string parentTag = pParentNameCast->GetValue();
			string childTag = pChildNameCast->GetValue();

			Entity * pParent = NULL;
			Entity * pChild = NULL;

			EntitySystem * pEntitySystem = g_Engine.GetEntitySystem();
			assert(pEntitySystem);
			CORE_ERROR errParent = pEntitySystem->GetEntityByTag(&parentTag, &pParent);
			CORE_ERROR errChild = pEntitySystem->GetEntityByTag(&childTag, &pChild);
			assert(errParent == ERR_OK && errChild == ERR_OK);
			if (errParent == ERR_OK && errChild == ERR_OK)
			{
				CORE_BOOLEAN wasAdopted = Entity::AdoptChild(pParent, pChild);
				if (wasAdopted)
				{
					IEventManager * pEventManager = g_Engine.GetEventManager();
					assert(pEventManager);
					EntityLinkedEventData * pEventData = new EntityLinkedEventData();
					pEventData->m_ParentIdentifier = pParent->GetIdentifier();
					pEventData->m_ChildIdentifier = pChild->GetIdentifier();
					pEventManager->VQueueEvent(pEventData);
				}
			}
		}
	}
}

void
Console::OnEntitiesUnLinkHandler(ConsoleCommandParameterList * pParams) const
{
	CORE_ERROR errCode = ERR_OK;
	CORE_BOOLEAN wasUnLinked = false;

	assert(pParams);
	assert(pParams->size() == 1);

	if (pParams && pParams->size() == 1)
	{
		ConsoleCommandParameterBase * pEntityName = pParams->at(0);
		assert(pEntityName);
		assert(pEntityName->m_Type == PARAM_STRING && pEntityName->m_pStrName == "entity_name");


		if (pEntityName && pEntityName->m_Type == PARAM_STRING && pEntityName->m_pStrName == "entity_name")
		{
			ConsoleCommandParameter<string> * pEntityNameCast = (ConsoleCommandParameter<string> *) pEntityName;

			string entityTag = pEntityNameCast->GetValue();

			Entity * pParent = NULL;
			Entity * pChild = NULL;

			EntitySystem * pEntitySystem = g_Engine.GetEntitySystem();
			assert(pEntitySystem);
			CORE_ERROR errChild = pEntitySystem->GetEntityByTag(&entityTag, &pChild);
			assert(errChild == ERR_OK);
			if (errChild == ERR_OK)
			{
				pParent = pChild->GetParent();
				if (pParent)
				{
					CORE_ID parentIdentifier = pParent->GetIdentifier();
					CORE_ERROR errParent = pEntitySystem->GetEntityByIdentifier(pParent->GetIdentifier(), &pParent);
					assert(errParent == ERR_OK);
					wasUnLinked = Entity::OrphanChild(pChild);
					if (wasUnLinked)
					{
						EntityUnLinkedEventData * pEventData = new EntityUnLinkedEventData();
						pEventData->m_ChildIdentifier = pChild->GetIdentifier();
						pEventData->m_ParentIdentifier = parentIdentifier;
						
						IEventManager * pEventManager = g_Engine.GetEventManager();
						assert(pEventManager);
						pEventManager->VQueueEvent(pEventData);
					}
				}
			}
		}
	}
}