#pragma once

#include "IEventData.h"
#include "CoreTypes.h"

struct KeyDownEventData : EventDataBase
{
	KEYCODE m_KeyCode;

	virtual IEventData * VCopy(void) const;
	virtual const char * VGetName() const;
	float EventDataBase::VGetTimeStamp(void) const;

	virtual const EventType& VGetEventType(void) const;

	virtual ~KeyDownEventData();

	virtual void EventDataBase::VSerialize(std::ostream & out) const;
};

struct KeyUpEventData : EventDataBase
{
	KEYCODE m_KeyCode;

	virtual IEventData * VCopy(void) const;

	virtual const char * VGetName() const;

	float EventDataBase::VGetTimeStamp(void) const;

	virtual const EventType& VGetEventType(void) const;

	virtual ~KeyUpEventData();

	virtual void EventDataBase::VSerialize(std::ostream & out) const;
};

struct KeyPressEventData : EventDataBase
{
	KEYCODE m_KeyCode;

	virtual IEventData * VCopy(void) const;
	virtual const char * VGetName() const;

	float EventDataBase::VGetTimeStamp(void) const;

	virtual const EventType& VGetEventType(void) const;

	virtual ~KeyPressEventData();

	virtual void EventDataBase::VSerialize(std::ostream & out) const;
};
