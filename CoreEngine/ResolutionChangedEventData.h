#pragma once

#include "IEventData.h"


struct ResolutionChangedEventData : EventDataBase
{
	CORE_DWORD m_Width;
	CORE_DWORD m_Height;

	virtual IEventData * VCopy(void) const
	{
		ResolutionChangedEventData * pData = new ResolutionChangedEventData();
		pData->m_Width = this->m_Width;
		pData->m_Height = this->m_Height;


		return (IEventData *)pData;
	}

	virtual const char * VGetName() const
	{
		return "ResolutionChangedEventData";
	}

	float EventDataBase::VGetTimeStamp(void) const
	{
		return this->m_timeStamp;
	}

	virtual const EventType& VGetEventType(void) const
	{
		return EventType::EVENT_TYPE_RESOLUTION_CHANGED;
	}

	virtual ~ResolutionChangedEventData()
	{
	}

	virtual void EventDataBase::VSerialize(std::ostream &out) const
	{
		//TODO something.
		out << this->m_Width + "x" << this->m_Height;
	}
};