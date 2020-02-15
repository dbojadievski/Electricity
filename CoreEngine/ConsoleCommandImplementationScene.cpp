#include "CoreTypes.h"
#include "CoreEngine.h"
#include <string>
#include "Console.h"
//#include "tinyxml2.h"
using namespace std;
//using namespace tinyxml2;

#include "IComponent.h"
#include "TransformComponent.h"
#include "RenderableComponent.h"

#include "SceneEvents.h"

void
ProcessEntityComponent(XMLElement * pComponentXML, Console * pConsole, __PARAM_OUT__ Entity * pEntity)
{
	assert(pComponentXML);
	assert(pEntity);

	IAssetManager * pAssetManager = g_Engine.GetAssetManager();
	string type = pComponentXML->Attribute("type");
	EComponentType componentType = IComponent::StringToType(type);

	IComponent * pComponent = NULL;
	switch (componentType)
	{
	case COMPONENT_TYPE_TRANSFORM:
		pComponent = new TransformComponent(pComponentXML);
		break;
	case COMPONENT_TYPE_RENDERABLE:
		pComponent = new RenderableComponent(pComponentXML, pAssetManager);
		break;
	default:
		assert(false);
		break;
	}

	if (pComponent)
		g_Engine.GetEntitySystem()->RegisterComponent(pEntity, pComponent);
}

void 
ProcessEntity(XMLElement * pEntity, __PARAM_OUT__ vector<Entity *> * pEntities, Console  * pConsole, Entity * pParent = NULL)
{
	/*
	* NOTE(Dino): Entities consist of two things:
	* Components, which attach to them, and
	* Child entities.
	*/
	assert(pEntity);
	assert(pEntities);

	string name = pEntity->Attribute("name");
	Entity * pEntityObj = new Entity(new string(name));
	g_Engine.GetEntitySystem()->RegisterEntity(pEntityObj);
	pEntities->push_back(pEntityObj);
	if (pParent)
	{
		CORE_BOOLEAN linkResult;
		g_Engine.GetEntitySystem()->Link(pParent, pEntityObj, &linkResult);
		assert(linkResult);
	}


	//First, we parse components.
	XMLElement * pComponents = pEntity->FirstChildElement("components");
	assert(pComponents);
	XMLElement * pComponent = pComponents->FirstChildElement();
	while (pComponent)
	{
		ProcessEntityComponent(pComponent, pConsole, pEntityObj);
		pComponent = pComponent->NextSiblingElement();
	}

	//Next up, the children!
	XMLElement * pChildList = pEntity->FirstChildElement("entities");
	if (pChildList)
	{
		XMLElement *pChild = pChildList->FirstChildElement("entity");
		while (pChild)
		{
			ProcessEntity(pChild, pEntities, pConsole, pEntityObj);
			pChild = pChild->NextSiblingElement("entity");
		}
	}


}

void
Console::OnSceneLoadHandler(ConsoleCommandParameterList * pParams) const
{
	CORE_ERROR errCode = ERR_OK;
	assert(pParams);
	assert(pParams->size() == 1);

	ConsoleCommandParameterBase * pName = pParams->at(0);
	assert(pName);
	CORE_BOOLEAN isParamValid = (pName && pName->m_Type == PARAM_STRING && pName-> m_pStrName == "scene_name");
	assert(isParamValid);
	if (isParamValid)
	{
		ConsoleCommandParameter<string> * pNameCast = (ConsoleCommandParameter<string> *) pName;
		EntitySystem * pEntitySystem = g_Engine.GetEntitySystem();
		assert(pEntitySystem);

		tinyxml2::XMLDocument doc;
		string scenePath = "Assets\\Scenes\\" + pNameCast->GetValue() + ".xml";
		const char * pPath = scenePath.c_str();
		doc.LoadFile(pPath);
		
		XMLElement * pXmlSceneList = doc.FirstChildElement("scenes");
		if (!pXmlSceneList || pXmlSceneList->NoChildren())
			assert(false);

		XMLNode * pScenes = pXmlSceneList->FirstChild();
		while (pScenes)
		{
			XMLElement * pScene = pScenes->ToElement();
			assert(pScene);

			string name = pScene->Attribute("name");
			if (name == pNameCast->GetValue())
			{
				vector<Entity *> * pEntityList = new vector<Entity *>();
				XMLElement * pEntities = pScene->FirstChildElement("entities");
				XMLElement * pEntity = pEntities->FirstChildElement("entity");
				while (pEntity)
				{
					ProcessEntity(pEntity, pEntityList, (Console *) this);
					pEntity = pEntity->NextSiblingElement();
				}

				
				SceneLoadedEventData * pEvent = new SceneLoadedEventData();
				pEvent->m_pEntities = pEntityList;
				g_Engine.GetEventManager()->VQueueEvent(pEvent);
				break;
			}

			pScenes = pScenes->NextSibling();
		}
	}
}