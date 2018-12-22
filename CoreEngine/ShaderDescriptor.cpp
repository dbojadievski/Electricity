#include "ShaderDescriptor.h"

ShaderAssetDescriptor::ShaderAssetDescriptor(const string & path, CORE_ID identifier)
{
	this->m_Path = path;
	this->m_Identifier = identifier;
}

CORE_ASSET_TYPE
ShaderAssetDescriptor::GetAssetType()
{
	return this->m_AssetType;
}


ShaderAssetDescriptorExtended::ShaderAssetDescriptorExtended(const string & path, CORE_ID identifier, const string & shaderSource) : ShaderAssetDescriptor(path, identifier)
{
	this->m_ShaderSource = shaderSource;
}

string
ShaderAssetDescriptorExtended::GetSource()
{
	return this->m_ShaderSource;
}