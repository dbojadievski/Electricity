#include "ResolutionChangedEventData.h"

IEventData * 
ResolutionChangedEventData::VCopy(void) const
{
	ResolutionChangedEventData * pData = new ResolutionChangedEventData();
	pData->m_Width = this->m_Width;
	pData->m_Height = this->m_Height;


	return (IEventData *)pData;
}

const char * 
ResolutionChangedEventData::VGetName() const
{
	return "ResolutionChangedEventData";
}

float ResolutionChangedEventData::VGetTimeStamp(void) const
{
	return this->m_timeStamp;
}

const EventType& 
ResolutionChangedEventData::VGetEventType(void) const
{
	return EventType::EVENT_TYPE_RESOLUTION_CHANGED;
}

ResolutionChangedEventData::~ResolutionChangedEventData()
{
}

void 
ResolutionChangedEventData::VSerialize(std::ostream &out) const
{
	//TODO something.
	out << this->m_Width + "x" << this->m_Height;
}