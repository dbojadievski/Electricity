#include "Console.h"
#include "CoreHeaders.h"
#include "CoreEngine.h"
#include <string>

using namespace std;

void
Console::OnEntityCreateHandler(ConsoleCommandParameterList * pParams) const
{
	assert(pParams);
	assert(pParams->size());

	ConsoleCommandParameterBase * pName = pParams->at(0);
	assert(pName);
	assert(pName->m_Type == PARAM_STRING && pName->m_pStrName == "entity_name");
	if (pName && pName->m_Type == PARAM_STRING && pName->m_pStrName == "entity_name")
	{
		ConsoleCommandParameter<string> * pNameCast = (ConsoleCommandParameter<string> *) pParams->at(0);
		EntitySystem * pEntitySystem = g_Engine.GetEntitySystem();

		string * pStrEntityName = new string(pNameCast->GetValue());
		Entity * pEntity = new Entity(pStrEntityName);
		pEntitySystem->RegisterEntity(pEntity);
		assert(pEntitySystem);
	}
}
