#include "AssetDescriptor.h"

string
AssetDescriptor::GetName () const
{
    return this->m_Name;
}

string 
AssetDescriptor::GetPath() const
{
	return this->m_Path;
}

CORE_ID 
AssetDescriptor::GetIdentifier() const
{
	return this->m_Identifier;
}

CORE_ASSET_TYPE 
AssetDescriptor::GetAssetType() const
{
	return this->m_AssetType;
}
