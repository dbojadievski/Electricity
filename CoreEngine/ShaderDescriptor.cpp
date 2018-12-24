#include "ShaderDescriptor.h"

ShaderAssetDescriptor::ShaderAssetDescriptor(const string & path, CORE_ID identifier)
{
    this->m_AssetType   = ASSET_TYPE_SHADER;

	this->m_Path        = path;
	this->m_Identifier  = identifier;
}

CORE_ASSET_TYPE
ShaderAssetDescriptor::GetAssetType()
{
	return this->m_AssetType;
}


ShaderAssetDescriptorExtended::ShaderAssetDescriptorExtended(const string & path, CORE_ID identifier) : ShaderAssetDescriptor(path, identifier)
{
}