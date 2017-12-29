#pragma once

#include "IEventData.h"

#include "IEventData.h"
#include "inputHeaders.h"

struct MouseButtonPressedEventData : EventDataBase
{
	MouseButton m_Button;

	virtual IEventData * VCopy(void) const;

	virtual const char * VGetName() const;
	float EventDataBase::VGetTimeStamp(void) const;
	virtual const EventType& VGetEventType(void) const;

	virtual ~MouseButtonPressedEventData();
	virtual void EventDataBase::VSerialize(std::ostream & out) const;
};
struct MouseButtonDownEventData : EventDataBase
{
	MouseButton m_Button;

	virtual IEventData * VCopy(void) const;
	virtual void EventDataBase::VSerialize(std::ostream & out) const;

	virtual const char * VGetName() const;
	float EventDataBase::VGetTimeStamp(void) const;
	virtual const EventType& VGetEventType(void) const;

	virtual ~MouseButtonDownEventData();

};
struct MouseButtonReleasedEventData : EventDataBase
{
	MouseButton m_Button;

	virtual IEventData * VCopy(void) const;
	virtual void EventDataBase::VSerialize(std::ostream & out) const;

	virtual const char * VGetName() const;
	float EventDataBase::VGetTimeStamp(void) const;
	virtual const EventType& VGetEventType(void) const;

	virtual ~MouseButtonReleasedEventData();

};