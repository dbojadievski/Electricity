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
	this->m_pTag = (string *)pTag;
	this->m_pParent = (GameObject *)pParent;
}

void 
GameObject::Init()
{
	for (auto componentIterator = this->m_Components.begin(); componentIterator != this->m_Components.end(); componentIterator++)
	{
		IComponent * pComponent = componentIterator->second;
		pComponent->Init();
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
	for (auto componentIterator = this->m_Components.begin(); componentIterator != this->m_Components.end(); componentIterator++)
	{
		IComponent * pComponent = componentIterator->second;
		pComponent->Update(dT);
	}

	size_t numChildren = this->m_Children.size();
	for (size_t currChildIdx = 0; currChildIdx < numChildren; currChildIdx++)
	{
		GameObject * pChild = this->m_Children[currChildIdx];
		assert(pChild);
		pChild->Update(dT);
	}
}

/* * * * * * * * * * * * * * * * * * * * * * */
/* Object component management.              */
/* * * * * * * * * * * * * * * * * * * * * * */
IComponent *
GameObject::GetComponentByType(const EComponentType componentType)
{
	IComponent * retVal = NULL;

	map<EComponentType, IComponent *>::iterator it = this->m_Components.find(componentType);
	if (it != this->m_Components.end())
		retVal = this->m_Components[componentType];

	return retVal;
}

void
GameObject::RegisterComponent(const IComponent * pComponent)
{
	assert(pComponent);

	map<EComponentType, IComponent *>::iterator it = this->m_Components.find(pComponent->m_Type);
	CORE_BOOLEAN hasComponentOfType = (it != this->m_Components.end());
	if (!hasComponentOfType)
		this->m_Components.insert(std::make_pair(pComponent->m_Type, (IComponent *)pComponent));
	else
		it->second = (IComponent *)pComponent;
}

void
GameObject::UnregisterComponent(const EComponentType componentType)
{
	map<EComponentType, IComponent *>::iterator it = this->m_Components.find(componentType);
	if (it != this->m_Components.end())
		this->m_Components.erase(componentType);
}


/* * * * * * * * * * * * * * * * * * * * * * */
/* Object parent-child dependency management.*/
/* * * * * * * * * * * * * * * * * * * * * * */
void
GameObject::AdoptChild(GameObject * pParent, GameObject * pChild)
{
	assert(pParent);
	assert(pChild);
	assert(pParent != pChild);
	assert(pParent->m_pParent != pChild);
	assert(pChild->m_pParent != pParent);

	if (!pParent || !pChild)
		goto end;

	if (pChild == pParent)
		goto end;

	if (pChild->m_pParent == pParent || pParent->m_pParent == pChild)
		goto end;

	pChild->m_pParent = pParent;
	pParent->m_Children.push_back(pChild);

end:
	return;
}

void
GameObject::OrphanChild(GameObject * pChild)
{
	assert(pChild);
	assert(pChild->m_pParent);
	assert(pChild->m_pParent != pChild);

	GameObject * pParent = pChild->m_pParent;
	pChild->m_pParent = NULL;
	
	size_t numChildren = pParent->m_Children.size();
	for (size_t currIdx = 0; currIdx < numChildren; currIdx++)
		if (pParent->m_Children[currIdx] == pChild)
		{
			pParent->m_Children.erase(pParent->m_Children.begin() + currIdx);
			break;
		}
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




