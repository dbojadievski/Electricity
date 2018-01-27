#pragma once

#include "IEventData.h"

struct EntityUnRegisteredEventData : EventDataBase
{
	CORE_ID m_Identifier;

	virtual IEventData * VCopy(void) const;
	virtual const char * VGetName() const;

	float EventDataBase::VGetTimeStamp(void) const;

	virtual const EventType& VGetEventType(void) const;

	
	virtual ~EntityUnRegisteredEventData();
	virtual void EventDataBase::VSerialize(std::ostream &out) const;
};

struct EntityRegisteredEventData : EventDataBase
{
	CORE_ID m_Identifier;

	virtual IEventData * VCopy(void) const;
	virtual const char * VGetName() const;

	float EventDataBase::VGetTimeStamp(void) const;

	virtual const EventType& VGetEventType(void) const;


	virtual ~EntityRegisteredEventData();
	virtual void EventDataBase::VSerialize(std::ostream &out) const;
};