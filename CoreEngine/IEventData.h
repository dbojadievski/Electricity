#pragma once
#include "CoreTypes.h"
#include "FastDelegate.h"

#include <ostream>
typedef unsigned long EventType;

class IEventData
{
public:
	virtual const EventType& VGetEventType(void) const = 0;
	virtual float VGetTimeStamp(void) const = 0;
	virtual void VSerialize(std::ostream & out) const = 0;
	virtual IEventData * VCopy(void) const = 0;
	virtual const char * VGetName(void) const = 0;
};

class EventDataBase : public IEventData
{
protected:
	const CORE_REAL m_timeStamp;
public:
	explicit EventDataBase(const CORE_REAL timeStamp = 0.0f) : m_timeStamp(timeStamp) {}
	//virtual ~EventDataBase(void) = 0;

	virtual const EventType& VGetEventType(void) const = 0;
	float VGetTimeStamp(void)const = 0;

	/* Used for network transfer.*/
	virtual void VSerialize(std::ostream &out) const = 0;
};


typedef fastdelegate::FastDelegate1<IEventData *> EventListenerDelegate;