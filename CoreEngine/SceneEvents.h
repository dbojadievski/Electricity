#pragma once

#include "IEventData.h"
#include <vector>
#include "GameObject.h"

using std::vector;
struct SceneLoadedEventData : EventDataBase 
{
	vector<Entity *> * m_pEntities;
	virtual IEventData * VCopy(void) const;
	virtual const char * VGetName() const;

	float EventDataBase::VGetTimeStamp(void) const;

	virtual const EventType VGetEventType(void) const;

	virtual void EventDataBase::VSerialize(std::ostream & out) const;
};