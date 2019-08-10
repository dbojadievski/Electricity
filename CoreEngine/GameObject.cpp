#pragma once

/* * * * * * * * * * * * * * * * * * * * * * */
/* @Author Dino 'Sourcery' Bojadjievski		 */
/* @Package Electricity						 */
/* @Filename GameObject.cpp					 */
/* * * * * * * * * * * * * * * * * * * * * * */

#include <assert.h>
#include <vector>
#include <map>

#include "CoreTypes.h"
#include "IComponent.h"
#include "GameObject.h"

using namespace std;

CORE_ID
GameObject::GetIdentifier() const
{
	return this->m_Identifier;
}

/* * * * * * * * * * * * * * * * * * * * * * */
/* Object generic functionality.             */
/* * * * * * * * * * * * * * * * * * * * * *   */

GameObject::GameObject(const string * pTag, const GameObject * pParent)
{	
	this->m_pTag = new string( *pTag);
	this->m_pParent = (GameObject *)pParent;
}

void 
GameObject::Init()
{
	for (auto componentListIterator = this->m_Components.begin(); componentListIterator != this->m_Components.end(); componentListIterator++)
	{
		ComponentList * pComponentList = componentListIterator->second;
		for (auto componentIterator = pComponentList->begin(); componentIterator != pComponentList->end(); componentIterator++)
		{
			IComponent * pComponent = *componentIterator;
			assert(pComponent);
			pComponent->Init();
		}
	}

	for (size_t currChildIdx = 0; currChildIdx < this->m_Children.size(); currChildIdx++)
	{
		GameObject * pChild = this->m_Children[currChildIdx];
		assert(pChild);
		pChild->Init();
	}
}

void 
GameObject::Update(float dT)
{
	for (auto componentListIterator = this->m_Components.begin(); componentListIterator != this->m_Components.end(); componentListIterator++)
	{
		ComponentList * pComponentList = componentListIterator->second;
		for (auto componentIterator = pComponentList->begin(); componentIterator != pComponentList->end(); componentIterator++)
		{
			IComponent * pComponent = *componentIterator;
			assert(pComponent);
			pComponent->Update(dT);
		}
	}

	size_t numChildren = this->m_Children.size();
	for (size_t currChildIdx = 0; currChildIdx < numChildren; currChildIdx++)
	{
		GameObject * pChild = this->m_Children[currChildIdx];
		assert(pChild);
		pChild->Update(dT);
	}
}

GameObject * const
GameObject::GetParent() const
{
	return this->m_pParent;
}

string const *
GameObject::GetTag() const
{
	return this->m_pTag;
}

/* * * * * * * * * * * * * * * * * * * * * * */
/* Object component management.              */
/* * * * * * * * * * * * * * * * * * * * * * */
IComponent *
GameObject::GetComponentByType(const EComponentType componentType)
{
	IComponent * retVal = NULL;

	auto it = this->m_Components.find(componentType);
	if (it != this->m_Components.end())
	{
		ComponentList * pComponentList = this->m_Components[componentType];
		retVal = pComponentList->at(0);
	}

	return retVal;
}

CORE_BOOLEAN
GameObject::RegisterComponent(IComponent * pComponent)
{
	assert(pComponent);
	CORE_BOOLEAN wasRegistered = false;

	ComponentList * pComponentList = NULL;

	auto componentsOfTypeIt = this->m_Components.find(pComponent->m_Type);
	CORE_BOOLEAN hasComponentOfType = (componentsOfTypeIt != this->m_Components.end());
	if (!hasComponentOfType)
	{
		pComponentList = new ComponentList();
		this->m_Components.insert(std::make_pair(pComponent->m_Type, pComponentList));
	}
	else
		pComponentList = componentsOfTypeIt->second;
	
	size_t numComponentsOfType = pComponentList->size();
	CORE_BOOLEAN areMultipleInstancesAllowed = pComponent->AreMultipleInstancesAllowed();
	
	if (numComponentsOfType == 0 || areMultipleInstancesAllowed)
	{
		pComponentList->push_back((IComponent *) pComponent);
		//pComponent->m_pOwner = this;
		wasRegistered = true;
	}

	return wasRegistered;
}

CORE_BOOLEAN
GameObject::UnregisterComponent(EComponentType componentType, CORE_ID identifier)
{
	CORE_BOOLEAN wasUnregistered = false;

	auto componentListIt = this->m_Components.find(componentType);
	if (componentListIt != this->m_Components.end())
	{
		ComponentList * pComponentList = componentListIt->second;
		assert(pComponentList);
		assert(pComponentList->size());
		size_t numComponentsOfType = pComponentList->size();
		if (numComponentsOfType)
		{
			if (identifier == COMPONENT_DELETE_ALL || numComponentsOfType == 1)
			{
				for (size_t currComponentIdx = 0; currComponentIdx < numComponentsOfType; currComponentIdx++)
				{
					IComponent * pComponent = pComponentList->at(currComponentIdx);
					assert(pComponent);
					delete pComponent;
					wasUnregistered = true;
				}

				delete pComponentList;
				this->m_Components.erase(componentType);
			}
			else
			{
				for (size_t currComponentIdx = 0; currComponentIdx < numComponentsOfType; currComponentIdx++)
				{
					IComponent * pComponent = pComponentList->at(currComponentIdx);
					assert(pComponent);
					if (pComponent->m_Identifier == identifier)
					{
						/*Delete component.*/
						pComponentList->erase(pComponentList->begin() + currComponentIdx);
						delete pComponent;
						wasUnregistered = true;
						break;
					}
				}
			}
		}
	}

	return wasUnregistered;
}

/* * * * * * * * * * * * * * * * * * * * * * */
/* Object parent-child dependency management.*/
/* * * * * * * * * * * * * * * * * * * * * * */
CORE_BOOLEAN
GameObject::AdoptChild(GameObject * pParent, GameObject * pChild)
{
	assert(pParent);
	assert(pChild);
	assert(pParent != pChild);
	assert(pParent->m_pParent != pChild);
	assert(pChild->m_pParent != pParent);

	CORE_BOOLEAN areRelated = false;

	if (!pParent || !pChild)
		goto end;

	if (pChild == pParent)
		goto end;

	if (pChild->m_pParent == pParent || pParent->m_pParent == pChild)
		goto end;

	pChild->m_pParent = pParent;
	pParent->m_Children.push_back(pChild);
	
	areRelated = true;
end:
	return areRelated;
}

CORE_BOOLEAN
GameObject::OrphanChild(GameObject * pChild)
{
	assert(pChild);
	assert(pChild->m_pParent);
	assert(pChild->m_pParent != pChild);

	CORE_BOOLEAN wasOrphaned = false;

	GameObject * pParent = pChild->m_pParent;
	if (pParent)
	{
		pChild->m_pParent = NULL;
	
		size_t numChildren = pParent->m_Children.size();
		for (size_t currIdx = 0; currIdx < numChildren; currIdx++)
			if (pParent->m_Children[currIdx] == pChild)
			{
				pParent->m_Children.erase(pParent->m_Children.begin() + currIdx);
				wasOrphaned = true;
				break;
			}
	}

	return wasOrphaned;
}

CORE_BOOLEAN
GameObject::AreRelated(const GameObject * pPotentialAncestor, const GameObject * pPotentialDescendant)
{
	assert(pPotentialAncestor && pPotentialDescendant);

	CORE_BOOLEAN areRelated = false;

	if (!pPotentialAncestor || !pPotentialDescendant)
		goto end;

	GameObject * pCastAncestor = (GameObject *)pPotentialDescendant->m_pParent;
	while (pCastAncestor && pCastAncestor != pPotentialAncestor)
		pCastAncestor = pCastAncestor->m_pParent;

	areRelated = pCastAncestor;

	end:
	return areRelated;
}




