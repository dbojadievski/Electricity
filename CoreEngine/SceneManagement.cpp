#include "Math.h"
#include "SceneManagement.h"
#include "EntitySystem.h"

CORE_ERROR
LoadScene(XMLElement  * pScene)
{
	assert(pScene);
	if (pScene && pScene->Name() == "scene")
	{
		string name;
		vector<Entity> * pEntities = new vector<Entity>();

	}

	return ERR_OK;
}