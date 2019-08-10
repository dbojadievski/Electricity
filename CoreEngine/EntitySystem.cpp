#include "EntitySystem.h"
#include "EntityEvents.h"

CORE_ID EntitySystem::s_NextEntityIdentifier = 0;

EntitySystem::EntitySystem(IEventManager * pEventManager)
{
	assert(pEventManager);
	this->m_pEventManager = pEventManager;
}

#pragma region Elementary entity management.
CORE_ERROR
EntitySystem::RegisterEntity(Entity * pEntity)
{
	assert(pEntity);

	CORE_ERROR errCode = CORE_ERROR::ERR_SUCCESS;

	if (!pEntity)
	{
		errCode = CORE_ERROR::ERR_PARAM_INVALID;
	}
	else
	{
		pEntity->m_Identifier = ++EntitySystem::s_NextEntityIdentifier;

		auto pair = make_pair(pEntity->m_Identifier, pEntity);
		this->m_Entities.insert(pair);

		EntityRegisteredEventData * pEntityRegisteredEventData = new EntityRegisteredEventData();
		pEntityRegisteredEventData->m_Identifier = pEntity->m_Identifier;
		this->m_pEventManager->VQueueEvent(pEntityRegisteredEventData);
	}
	
	return errCode;
}

CORE_ERROR
EntitySystem::UnRegisterEntity(CORE_ID identifier)
{
	assert(identifier);
	
	CORE_ERROR errCode = CORE_ERROR::ERR_OBJECT_NOT_FOUND;
	if (identifier)
	{
		size_t numErased = this->m_Entities.erase(identifier);
		if (numErased)
		{
			errCode = CORE_ERROR::ERR_SUCCESS;

			EntityUnRegisteredEventData * pEntityDeregisteredEventData = new EntityUnRegisteredEventData();
			pEntityDeregisteredEventData->m_Identifier = identifier;
			this->m_pEventManager->VQueueEvent(pEntityDeregisteredEventData);
		}
	}

	return errCode;
}

CORE_ERROR
EntitySystem::Link(GameObject * pParent, GameObject * pChild, __PARAM_OUT__ CORE_BOOLEAN * pResult)
{
	CORE_ERROR errCode = CORE_ERROR::ERR_OK;
	assert(pParent);
	assert(pChild);

	if (pParent && pChild)
	{
		this->UnLink(pChild, pResult);
		*pResult = GameObject::AdoptChild(pParent, pChild);
		if (*pResult)
		{
			EntityLinkedEventData * pEntityLinkedEventData = new EntityLinkedEventData();
			pEntityLinkedEventData->m_ChildIdentifier = pChild->GetIdentifier();
			pEntityLinkedEventData->m_ParentIdentifier = pParent->GetIdentifier();
			this->m_pEventManager->VQueueEvent(pEntityLinkedEventData);
		}
	}
	else
		errCode = CORE_ERROR::ERR_PARAM_INVALID;

	return errCode;
}

CORE_ERROR
EntitySystem::UnLink(GameObject * pChild, __PARAM_OUT__ CORE_BOOLEAN * pResult)
{
	CORE_ERROR errCode = CORE_ERROR::ERR_OK;
	assert(pChild);

	if (pChild)
	{
		CORE_ID parentID = -1;
		if (pChild->m_pParent)
			parentID = pChild->GetParent()->GetIdentifier();
		*pResult = GameObject::OrphanChild(pChild);
		if (*pResult)
		{
			EntityUnLinkedEventData * pEntityUnLinkedEventData = new EntityUnLinkedEventData();
			pEntityUnLinkedEventData->m_ChildIdentifier = pChild->GetIdentifier();
			pEntityUnLinkedEventData->m_ParentIdentifier = parentID;
			this->m_pEventManager->VQueueEvent(pEntityUnLinkedEventData);
		}
	}

	return errCode;
}

CORE_ERROR
EntitySystem::GetEntityByIdentifier(CORE_ID identifier, __PARAM_OUT__ Entity ** pEntity)
{
	assert(identifier);

	CORE_ERROR errCode = CORE_ERROR::ERR_SUCCESS;

	if (identifier)
	{
		auto findIt = this->m_Entities.find(identifier);
		if (findIt != this->m_Entities.end())
			*pEntity = findIt->second;
		else
			errCode = CORE_ERROR::ERR_OBJECT_NOT_FOUND;
	}
	else
		errCode = CORE_ERROR::ERR_PARAM_OUT_OF_RANGE;

	return errCode;
}

CORE_ERROR
EntitySystem::GetEntityByTag(string const * pTag, __PARAM_OUT__ Entity ** pEntity)
{
	assert(pTag);

	CORE_ERROR errCode = CORE_ERROR::ERR_PARAM_INVALID;
	if (pTag)
	{
		auto entitiesIt = this->m_Entities.begin();
		while (entitiesIt != this->m_Entities.end())
		{
			Entity * pCurrEntity = entitiesIt->second;
			if (pCurrEntity->m_pTag && !pCurrEntity->m_pTag->compare(*pTag))
			{
				*pEntity = pCurrEntity;
				errCode = CORE_ERROR::ERR_SUCCESS;
				break;
			}
			entitiesIt++;
		}

		if (errCode != CORE_ERROR::ERR_SUCCESS)
			errCode = CORE_ERROR::ERR_OBJECT_NOT_FOUND;
	}

	return errCode;
}

CORE_BOOLEAN
EntitySystem::RegisterComponent(GameObject * pEntity, IComponent * pComponent)
{
	CORE_BOOLEAN wasRegistered = pEntity->RegisterComponent(pComponent);

	if (wasRegistered)
	{
		auto * pEventData = new EntityComponentAddedEventData();
		pEventData->m_ComponentType = pComponent->m_Type;
		pEventData->m_pComponent = pComponent;
		pEventData->m_EntityIdentifier = pEntity->GetIdentifier();
		this->m_pEventManager->VQueueEvent(pEventData);
	}

	return wasRegistered;
}

CORE_BOOLEAN
EntitySystem::UnRegisterComponent(GameObject * pEntity, IComponent * pComponent)
{
	CORE_BOOLEAN wasUnRegistered = pEntity->UnregisterComponent(pComponent->m_Type, pComponent->m_Identifier);
	if (wasUnRegistered)
	{
		//auto * pEventData = new EntityComponentRemovedEventData();
		//pEventData->m_ComponentType = pComponent->m_Type;
		//pEventData->m_pComponent = pComponent;
		//pEventData->m_EntityIdentifier = pEntity->GetIdentifier();
		//this->m_pEventManager->VQueueEvent(pEventData);
	}

	return wasUnRegistered;
}

#pragma endregion


#pragma region Elementary engine system events.
void
EntitySystem::Init(){}

void
EntitySystem::Update(CORE_DOUBLE dT) {}

void
EntitySystem::ShutDown() 
{
	this->m_Entities.clear();
	
	EntityUnRegisteredEventData * pEventData = new EntityUnRegisteredEventData();
	pEventData->m_Identifier = -1;
	this->m_pEventManager->VQueueEvent(pEventData);
}

#pragma endregion