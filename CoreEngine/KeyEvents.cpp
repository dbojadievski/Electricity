#include "KeyEvents.h"

#pragma region Keydown
/* KEYDOWN event data. */
IEventData *
KeyDownEventData::VCopy(void) const
{
	KeyDownEventData * pData = new KeyDownEventData();
	pData->m_KeyCode = this->m_KeyCode;

	return (IEventData *)pData;
}

const char *
KeyDownEventData::VGetName() const
{
	return "KeyEventData";

}

float 
KeyDownEventData::VGetTimeStamp(void) const
{
	return this->m_timeStamp;

}

const EventType&
KeyDownEventData::VGetEventType(void) const
{
	return EventType::EVENT_TYPE_KEYDOWN;
}

void
KeyDownEventData::VSerialize(std::ostream & out) const
{
	out << "KEYDOWN  " << this->m_KeyCode;

}

KeyDownEventData::~KeyDownEventData(){}
#pragma endregion

#pragma region Key Up

IEventData * 
KeyUpEventData::VCopy(void) const
{
	KeyUpEventData * pData = new KeyUpEventData();
	pData->m_KeyCode = this->m_KeyCode;

	return (IEventData *)pData;
}

const char * 
KeyUpEventData::VGetName() const
{
	return "KeyEventData";
}

float 
KeyUpEventData::VGetTimeStamp(void) const
{
	return this->m_timeStamp;
}

const EventType& 
KeyUpEventData::VGetEventType(void) const
{
	return EventType::EVENT_TYPE_KEYUP;
}

KeyUpEventData::~KeyUpEventData(){}

void 
KeyUpEventData::VSerialize(std::ostream & out) const
{
	out << "KEYPRESS " << this->m_KeyCode;
}
#pragma endregion

#pragma region Key press.
IEventData * 
KeyPressEventData::VCopy(void) const
{
	KeyPressEventData * pData = new KeyPressEventData();
	pData->m_KeyCode = this->m_KeyCode;

	return (IEventData *)pData;
}

const char *
KeyPressEventData::VGetName() const
{
	return "KeyEventData";
}

float 
KeyPressEventData::VGetTimeStamp(void) const
{
	return this->m_timeStamp;
}

const EventType& 
KeyPressEventData::VGetEventType(void) const
{
	return EventType::EVENT_TYPE_KEYPRESS;
}

void 
KeyPressEventData::VSerialize(std::ostream & out) const
{
	out << "KEYPRESS " << this->m_KeyCode;
}

KeyPressEventData::~KeyPressEventData() { }
#pragma endregion

