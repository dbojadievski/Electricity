#pragma once

#include "IEventData.h"

#include "IEventData.h"
#include "inputHeaders.h"

struct MouseButtonPressedEventData : EventDataBase
{
	MouseButton m_Button;

	virtual IEventData * VCopy(void) const
	{
		MouseButtonPressedEventData * pData = new MouseButtonPressedEventData();
		pData->m_Button = this->m_Button;

		return (IEventData *)pData;
	}

	virtual const char * VGetName() const
	{
		return "MouseButtonPressed";
	}

	float EventDataBase::VGetTimeStamp(void) const
	{
		return this->m_timeStamp;
	}

	virtual const EventType& VGetEventType(void) const
	{
		return EventType::EVENT_TYPE_MOUSE_BUTTON_PRESSED;
	}

	virtual ~MouseButtonPressedEventData()
	{
	}

	virtual void EventDataBase::VSerialize(std::ostream & out) const
	{
		out << "MOUSE_BUTTON_PRESSED  " << this->m_Button;
	}
};
struct MouseButtonDownEventData : EventDataBase
{
	MouseButton m_Button;

	virtual IEventData * VCopy(void) const
	{
		MouseButtonDownEventData * pData = new MouseButtonDownEventData();
		pData->m_Button = this->m_Button;

		return (IEventData *)pData;
	}

	virtual const char * VGetName() const
	{
		return "MouseButtonDown";
	}

	float EventDataBase::VGetTimeStamp(void) const
	{
		return this->m_timeStamp;
	}

	virtual const EventType& VGetEventType(void) const
	{
		return EventType::EVENT_TYPE_MOUSE_BUTTON_DOWN;
	}

	virtual ~MouseButtonDownEventData()
	{
	}

	virtual void EventDataBase::VSerialize(std::ostream & out) const
	{
		out << "MOUSE_BUTTON_DOWN  " << this->m_Button;
	}
};
struct MouseButtonReleasedEventData : EventDataBase
{
	MouseButton m_Button;

	virtual IEventData * VCopy(void) const
	{
		MouseButtonReleasedEventData * pData = new MouseButtonReleasedEventData();
		pData->m_Button = this->m_Button;

		return (IEventData *)pData;
	}

	virtual const char * VGetName() const
	{
		return "MouseButtonUp";
	}

	float EventDataBase::VGetTimeStamp(void) const
	{
		return this->m_timeStamp;
	}

	virtual const EventType& VGetEventType(void) const
	{
		return EventType::EVENT_TYPE_MOUSE_BUTTON_UP;
	}

	virtual ~MouseButtonReleasedEventData()
	{
	}

	virtual void EventDataBase::VSerialize(std::ostream & out) const
	{
		out << "MOUSE_BUTTON_UP  " << this->m_Button;
	}
};