#pragma once

#include "IEventData.h"


struct ResolutionChangedEventData : EventDataBase
{
	CORE_DWORD m_Width;
	CORE_DWORD m_Height;

	virtual IEventData * VCopy(void) const;
	virtual void EventDataBase::VSerialize(std::ostream &out) const;

	virtual const char * VGetName() const;
	float EventDataBase::VGetTimeStamp(void) const;
	virtual const EventType VGetEventType(void) const;
	
	virtual ~ResolutionChangedEventData();
};