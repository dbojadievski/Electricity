#include "AssetDescriptor.h"

string 
AssetDescriptor::GetPath()
{
	return this->m_Path;
}

CORE_ID 
AssetDescriptor::GetIdentifier()
{
	return this->m_Identifier;
}

CORE_ASSET_TYPE 
AssetDescriptor::GetAssetType()
{
	return this->m_AssetType;
}
