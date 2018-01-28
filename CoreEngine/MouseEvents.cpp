#include "MouseEvents.h"

#pragma region Mouse button pressed.
IEventData * 
MouseButtonPressedEventData::VCopy(void) const
{
	MouseButtonPressedEventData * pData = new MouseButtonPressedEventData();
	pData->m_Button = this->m_Button;

	return (IEventData *)pData;
}

const char * 
MouseButtonPressedEventData::VGetName() const
{
	return "MouseButtonPressed";
}

float
MouseButtonPressedEventData::VGetTimeStamp(void) const
{
	return this->m_timeStamp;
}

const EventType 
MouseButtonPressedEventData::VGetEventType(void) const
{
	return EventType::EVENT_TYPE_MOUSE_BUTTON_PRESSED;
}

MouseButtonPressedEventData::~MouseButtonPressedEventData() {}

void 
MouseButtonPressedEventData::VSerialize(std::ostream & out) const
{
	out << "MOUSE_BUTTON_PRESSED  " << this->m_Button;
}
#pragma endregion.

#pragma region Mouse button down.
IEventData * 
MouseButtonDownEventData::VCopy(void) const
{
	MouseButtonDownEventData * pData = new MouseButtonDownEventData();
	pData->m_Button = this->m_Button;

	return (IEventData *)pData;
}

const char * 
MouseButtonDownEventData::VGetName() const
{
	return "MouseButtonDown";
}

float 
MouseButtonDownEventData::VGetTimeStamp(void) const
{
	return this->m_timeStamp;
}

const EventType 
MouseButtonDownEventData::VGetEventType(void) const
{
	return EventType::EVENT_TYPE_MOUSE_BUTTON_DOWN;
}

MouseButtonDownEventData::~MouseButtonDownEventData()
{
}

void
MouseButtonDownEventData::VSerialize(std::ostream & out) const
{
	out << "MOUSE_BUTTON_DOWN  " << this->m_Button;
}
#pragma endregion

#pragma region Mouse button released.

IEventData *
MouseButtonReleasedEventData::VCopy(void) const
{
	MouseButtonReleasedEventData * pData = new MouseButtonReleasedEventData();
	pData->m_Button = this->m_Button;

	return (IEventData *)pData;
}

void 
MouseButtonReleasedEventData::VSerialize(std::ostream & out) const
{
	out << "MOUSE_BUTTON_UP  " << this->m_Button;
}

const char * 
MouseButtonReleasedEventData::VGetName() const
{
	return "MouseButtonUp";
}

float MouseButtonReleasedEventData
::VGetTimeStamp(void) const
{
	return this->m_timeStamp;
}

const EventType 
MouseButtonReleasedEventData::VGetEventType(void) const
{
	return EventType::EVENT_TYPE_MOUSE_BUTTON_UP;
}

MouseButtonReleasedEventData::~MouseButtonReleasedEventData(){}
#pragma endregion