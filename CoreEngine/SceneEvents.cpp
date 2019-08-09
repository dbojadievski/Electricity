#include "SceneEvents.h"

IEventData *
SceneLoadedEventData::VCopy(void) const
{
	return NULL;
}

const char *
SceneLoadedEventData::VGetName() const
{
	return "SceneLoadedEventData";
}

float
SceneLoadedEventData::VGetTimeStamp() const
{
	return this->m_timeStamp;
}

const EventType
SceneLoadedEventData::VGetEventType() const
{
	return EventType::EVENT_TYPE_SCENE_LOADED;
}
void 
SceneLoadedEventData::VSerialize(std::ostream & out) const
{

}