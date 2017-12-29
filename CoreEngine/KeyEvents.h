#pragma once

#include "IEventData.h"
#include "CoreTypes.h"

struct KeyDownEventData : EventDataBase
{
	KEYCODE m_KeyCode;

	virtual IEventData * VCopy(void) const
	{
		KeyDownEventData * pData = new KeyDownEventData();
		pData->m_KeyCode = this->m_KeyCode;

		return (IEventData *)pData;
	}

	virtual const char * VGetName() const
	{
		return "KeyEventData";
	}

	float EventDataBase::VGetTimeStamp(void) const
	{
		return this->m_timeStamp;
	}

	virtual const EventType& VGetEventType(void) const
	{
		return EventType::EVENT_TYPE_KEYDOWN;
	}

	virtual ~KeyDownEventData()
	{
	}

	virtual void EventDataBase::VSerialize(std::ostream & out) const
	{
		out << "KEYDOWN  " <<  this->m_KeyCode;
	}
};

struct KeyUpEventData : EventDataBase
{
	KEYCODE m_KeyCode;

	virtual IEventData * VCopy(void) const
	{
		KeyUpEventData * pData = new KeyUpEventData();
		pData->m_KeyCode = this->m_KeyCode;

		return (IEventData *)pData;
	}

	virtual const char * VGetName() const
	{
		return "KeyEventData";
	}

	float EventDataBase::VGetTimeStamp(void) const
	{
		return this->m_timeStamp;
	}

	virtual const EventType& VGetEventType(void) const
	{
		return EventType::EVENT_TYPE_KEYUP;
	}

	virtual ~KeyUpEventData()
	{
	}

	virtual void EventDataBase::VSerialize(std::ostream & out) const
	{
		out << "KEYPRESS " << this->m_KeyCode;
	}
};

struct KeyPressEventData : EventDataBase
{
	KEYCODE m_KeyCode;

	virtual IEventData * VCopy(void) const
	{
		KeyPressEventData * pData = new KeyPressEventData();
		pData->m_KeyCode = this->m_KeyCode;

		return (IEventData *)pData;
	}

	virtual const char * VGetName() const
	{
		return "KeyEventData";
	}

	float EventDataBase::VGetTimeStamp(void) const
	{
		return this->m_timeStamp;
	}

	virtual const EventType& VGetEventType(void) const
	{
		return EventType::EVENT_TYPE_KEYPRESS;
	}

	virtual ~KeyPressEventData()
	{
	}

	virtual void EventDataBase::VSerialize(std::ostream & out) const
	{
		out << "KEYPRESS " << this->m_KeyCode;
	}
};
