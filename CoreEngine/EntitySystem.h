#pragma once

#include "CoreHeaders.h"
#include "IEngineSystem.h"
#include "EventManager.h"

#include "GameObject.h"

class EntitySystem : public IEngineSystem
{
	/*
	 * NOTE(Dino):
	 * There are many ways to store entities in the world.
	 *
	 * One practical way is to partition the world into sub-spaces.
	 * Then, a list of entities in every space is maintained.
	 * This allows quick determination of which objects surround and influence other objects.
	 * 
	 * Another way is to simply store them in a (very basic) list.
	 * This allows simple creation and deletion of objects.
	 * We'll start here, and work our way up to a more complex representation as needs develop.
	 */
private:
	static CORE_ID s_NextEntityIdentifier;

	EntityMap m_Entities;
	IEventManager * m_pEventManager;

	void OnEntityLoaded(IEventData * pEvent);
public:
	EntitySystem(IEventManager * pEventManager);

#pragma region Elementary entity management.
	CORE_ERROR RegisterEntity(Entity * pEntity);
	CORE_ERROR UnRegisterEntity(CORE_ID identifier);
	
	CORE_ERROR GetEntityByIdentifier(CORE_ID identifier, __PARAM_OUT__ Entity ** pEntity);
	CORE_ERROR GetEntityByTag(string const * pTag,  __PARAM_OUT__ Entity ** pEntity);

	CORE_ERROR Link(GameObject * pParent, GameObject * pChild, __PARAM_OUT__ CORE_BOOLEAN * pResult);
	CORE_ERROR UnLink(GameObject * pChild, __PARAM_OUT__ CORE_BOOLEAN * pResult);

	CORE_BOOLEAN RegisterComponent(GameObject * pObject, IComponent * pComponent);
	CORE_BOOLEAN UnRegisterComponent(GameObject * pObject, IComponent * pComponent);
#pragma endregion


#pragma region Basic engine system events.
	virtual void IEngineSystem::Init();
	virtual void IEngineSystem::Update(CORE_DOUBLE dT);
	virtual void IEngineSystem::ShutDown();
#pragma endregion
};