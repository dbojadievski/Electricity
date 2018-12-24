#include "AssetEvents.h"

#pragma region Asset loaded event data.

IEventData *
AssetLoadedEventData::VCopy(void) const
{
	AssetLoadedEventData * pNewData = new AssetLoadedEventData(this->m_pAssetDescriptorExtended);
	return pNewData;
}

const char *
AssetLoadedEventData::VGetName() const
{
	return "AssetLoadedEventData";
}

AssetLoadedEventData::~AssetLoadedEventData()
{

}

float
AssetLoadedEventData::VGetTimeStamp() const
{
	return this->m_timeStamp;
}

const EventType
AssetLoadedEventData::VGetEventType() const
{
	return EventType::EVENT_TYPE_ASSET_LOADED;
}

void
AssetLoadedEventData::VSerialize(std::ostream & out) const
{
	out << "ENTITY_ASSET_LOADED " << this->m_pAssetDescriptorExtended;
}

const CORE_ASSET_TYPE
AssetLoadedEventData::GetAssetType ()
{
    return this->m_pAssetDescriptorExtended->GetAssetType ();
}

AssetLoadedEventData::AssetLoadedEventData(){}
AssetLoadedEventData::AssetLoadedEventData(const AssetDescriptor * pAssetDescriptor)
{
	assert(pAssetDescriptor);
	this->m_pAssetDescriptorExtended = (AssetDescriptor *) pAssetDescriptor;
}

const EventType
AssetLoadFailedEventData::VGetEventType (void) const
{
    return EventType::EVENT_TYPE_ASSET_LOAD_FAILED;
}
#pragma endregion