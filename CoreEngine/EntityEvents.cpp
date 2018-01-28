#include "EntityEvents.h"

#pragma region Entity unregistered event data.
IEventData * EntityUnRegisteredEventData::VCopy(void) const
{
	EntityUnRegisteredEventData * copy = new EntityUnRegisteredEventData();
	copy->m_Identifier = this->m_Identifier;

	return copy;
}

const char * 
EntityUnRegisteredEventData::VGetName() const
{
	return "EntityUnRegisteredEventData";
}

const EventType
EntityUnRegisteredEventData::VGetEventType() const
{
	return EventType::EVENT_TYPE_ENTITY_UNREGISTERED;
}

void 
EntityUnRegisteredEventData::VSerialize(std::ostream & out) const
{
	out << "ENTITY_UNREGISTERED " << this->m_Identifier;
}
EntityUnRegisteredEventData::~EntityUnRegisteredEventData() {}

float
EntityUnRegisteredEventData::VGetTimeStamp() const
{
	return this->m_timeStamp;
}

#pragma endregion
#pragma region Entity registered event data.
IEventData * 
EntityRegisteredEventData::VCopy(void) const
{
	EntityRegisteredEventData * copy = new EntityRegisteredEventData();
	copy->m_Identifier = this->m_Identifier;

	return copy;
}

const char *
EntityRegisteredEventData::VGetName() const
{
	return "EntityRegisteredEventData";
}

const EventType
EntityRegisteredEventData::VGetEventType() const
{
	return EventType::EVENT_TYPE_ENTITY_REGISTERED;
}

void
EntityRegisteredEventData::VSerialize(std::ostream & out) const
{
	out << "ENTITY_REGISTERED " << this->m_Identifier;
}

EntityRegisteredEventData::~EntityRegisteredEventData() {}

float
EntityRegisteredEventData::VGetTimeStamp() const
{
	return this->m_timeStamp;
}
#pragma endregion
#pragma region Entity linked event data.
IEventData *
EntityLinkedEventData::VCopy(void) const
{
	EntityLinkedEventData * pCopy = new EntityLinkedEventData();
	
	pCopy->m_ParentIdentifier = this->m_ParentIdentifier;
	pCopy->m_ChildIdentifier = this->m_ChildIdentifier;

	return pCopy;
}

const char *
EntityLinkedEventData::VGetName() const
{
	return "EntityLinkedEventData";
}

const EventType
EntityLinkedEventData::VGetEventType() const
{
	return EventType::EVENT_TYPE_ENTITY_LINKED;
}

void
EntityLinkedEventData::VSerialize(std::ostream & out) const
{
	out << "ENTITY_LINKED " << this->m_ParentIdentifier << " " << this->m_ChildIdentifier;
}

float
EntityLinkedEventData::VGetTimeStamp() const
{
	return this->m_timeStamp;
}
EntityLinkedEventData::~EntityLinkedEventData() { }
#pragma endregion
#pragma region Entity unlinked event data.
IEventData *
EntityUnLinkedEventData::VCopy(void) const
{
	EntityUnLinkedEventData * pCopy = new EntityUnLinkedEventData();

	pCopy->m_ParentIdentifier = this->m_ParentIdentifier;
	pCopy->m_ChildIdentifier = this->m_ChildIdentifier;

	return pCopy;
}

const char *
EntityUnLinkedEventData::VGetName() const
{
	return "EntityLinkedEventData";
}

const EventType
EntityUnLinkedEventData::VGetEventType() const
{
	return EventType::EVENT_TYPE_ENTITY_UNLINKED;
}

void
EntityUnLinkedEventData::VSerialize(std::ostream & out) const
{
	out << "ENTITY_UNLINKED " << this->m_ParentIdentifier << " " << this->m_ChildIdentifier;
}

float
EntityUnLinkedEventData::VGetTimeStamp() const
{
	return this->m_timeStamp;
}
EntityUnLinkedEventData::~EntityUnLinkedEventData() { }
#pragma endregion
