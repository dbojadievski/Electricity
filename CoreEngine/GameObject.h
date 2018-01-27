#pragma once
#include <assert.h>
#include <vector>
#include <map>

#include "CoreTypes.h"
#include "IComponent.h"

using namespace std;

class GameObject
{
	friend class EntitySystem;
private:
	string * m_pTag;
	CORE_ID m_Identifier;
	GameObject * m_pParent;
	vector<GameObject *> m_Children;
	map<EComponentType, IComponent *> m_Components;

public:
	GameObject(const string * pTag = NULL, const GameObject * pParent = NULL);

	void Init();
	void Update(float dT);

	IComponent * GetComponentByType(const EComponentType componentType);
	void RegisterComponent(const IComponent * pComponent);
	void UnregisterComponent(const EComponentType componentType);

	static void AdoptChild(GameObject * pParent, GameObject * pChild);
	static void OrphanChild(GameObject * pChild);
	static CORE_BOOLEAN AreRelated(const GameObject * pPotentialAncestor, const GameObject * pPotentialDescendant);
};

typedef GameObject Entity;
typedef std::map<CORE_ID, GameObject *> EntityMap;
typedef std::vector<GameObject *> EntityList;