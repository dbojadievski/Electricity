#include "Console.h"
#include "CoreHeaders.h"
#include "CoreEngine.h"
#include <string>
#include "EntityEvents.h"
#include "RenderableComponent.h"

//#include "AssetLoader.h"
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
				CORE_BOOLEAN wasAdopted = pEntitySystem->Link(pParent, pChild, &wasAdopted);
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
				pEntitySystem->UnLink(pChild, &wasUnLinked);
				pParent = pChild->GetParent();
			}
		}
	}
}

void
Console::OnRenderableInstantiateHandler(ConsoleCommandParameterList * pParams) const 
{
	CORE_BOOLEAN wasInstantiated = false;

	assert(pParams);
	assert(pParams->size() == 1);

	if (pParams && pParams->size() == 1)
	{
		ConsoleCommandParameterBase * pRenderableName = pParams->at(0);
		assert(pRenderableName);

		CORE_BOOLEAN isParamValid = ((pRenderableName->m_Type == PARAM_STRING) && (pRenderableName->m_pStrName == "renderable_name"));
		assert(isParamValid);
		if (isParamValid)
		{
			ConsoleCommandParameter<string> *pRenderableNameCast = (ConsoleCommandParameter<string> *) pRenderableName;
			string renderableTag = pRenderableNameCast->GetValue();

			AssetDescriptor * pRenderableDesc = NULL;
			auto desc = g_Engine.GetAssetManager()->GetMeshDescriptor(renderableTag);
			if (desc != NULL)
			{
				Entity * pEntity = new Entity(&renderableTag);
			
				vector<Renderable *> renderables;
				g_Engine.GetRenderSystem()->GetRenderablesByMesh(desc, &renderables);
				if (!renderables.empty())
				{
					Renderable * pRenderable = renderables.at(0);
					IComponent * pComponent = new RenderableComponent(pEntity, pRenderable);
					pComponent->m_Identifier = 1;
					//TODO(Dino): Register renderable component.
					pEntity->RegisterComponent(pComponent);
				}

				EntitySystem * pEntitySystem = g_Engine.GetEntitySystem();
				pEntitySystem->RegisterEntity(pEntity);
			}
		}
	}
}