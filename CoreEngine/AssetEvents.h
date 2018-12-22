#pragma once
#include "IEventData.h"
#include "AssetDescriptor.h"

struct AssetLoadedEventData : EventDataBase
{
	AssetDescriptor * m_pAssetDescriptorExtended;

	virtual IEventData * VCopy(void) const;
	virtual const char * VGetName() const;

	float EventDataBase::VGetTimeStamp(void) const;

	virtual const EventType VGetEventType(void) const;

	AssetLoadedEventData();
	AssetLoadedEventData(const AssetDescriptor * pAssetDescriptor);

	virtual ~AssetLoadedEventData( );
	virtual void EventDataBase::VSerialize(std::ostream & out) const;

};