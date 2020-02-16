#pragma once
#include "CoreTypes.h"
#include "FastDelegate.h"

#include <ostream>
enum EventType
{
	EVENT_TYPE_UNUSED = 0,
	EVENT_TYPE_TEST,
	EVENT_TYPE_RESOLUTION_CHANGED = 2,
	EVENT_TYPE_KEYDOWN,
	EVENT_TYPE_KEYUP,
	EVENT_TYPE_KEYPRESS,
	EVENT_TYPE_MOUSE_BUTTON_PRESSED,
	EVENT_TYPE_MOUSE_BUTTON_DOWN,
	EVENT_TYPE_MOUSE_BUTTON_UP,
	EVENT_TYPE_ENTITY_REGISTERED,
	EVENT_TYPE_ENTITY_UNREGISTERED,
	EVENT_TYPE_ENTITY_LINKED,
	EVENT_TYPE_ENTITY_UNLINKED,
	EVENT_TYPE_ENTITY_COMPONENT_ADDED,
	EVENT_TYPE_ENTITY_COMPONENT_REMOVED,
	EVENT_TYPE_ENTITY_COMPONENT_CHANGED,
	EVENT_TYPE_ASSET_LOADED,
    EVENT_TYPE_ASSET_LOAD_FAILED,
	EVENT_TYPE_SCENE_LOADED,
	EVENT_TYPE_SCENE_UNLOADED,
	EVENT_TYPE_SCENE_LOAD_FAILED
};


class IEventData
{
public:
	virtual const EventType VGetEventType(void) const = 0;
	virtual float VGetTimeStamp(void) const = 0;
	virtual void VSerialize(std::ostream & out) const = 0;
	virtual IEventData * VCopy(void) const = 0;
	virtual const char * VGetName(void) const = 0;
};

class EventDataBase : public IEventData
{
protected:
	const CORE_REAL m_timeStamp;
public:
	explicit EventDataBase(const CORE_REAL timeStamp = 0.0f) : m_timeStamp(timeStamp) {}
	//virtual ~EventDataBase(void) = 0;

	virtual const EventType VGetEventType(void) const = 0;
	float VGetTimeStamp(void)const = 0;

	/* Used for network transfer.*/
	virtual void VSerialize(std::ostream &out) const = 0;
};


typedef fastdelegate::FastDelegate1<IEventData *> EventListenerDelegate;