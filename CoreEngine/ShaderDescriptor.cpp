#include "ShaderDescriptor.h"

#include "ShaderType.h"
ShaderAssetDescriptor::ShaderAssetDescriptor(const string & name, const string & path, CORE_ID identifier)
{
    this->m_AssetType   = ASSET_TYPE_SHADER;

    this->m_Name        = name;
	this->m_Path        = path;
	this->m_Identifier  = identifier;
}

ShaderAssetDescriptor::ShaderAssetDescriptor (const string & name, const string & path, CORE_ID identifier, CORE_SHADER_TYPE shaderType, CORE_SHADER_INPUT_TYPE shaderInputType, const string & entryPoint) : ShaderAssetDescriptor(name, path, identifier)
{
    this->m_Name                = name;
    this->m_ShaderType          = shaderType;
    this->m_EntryPoint          = entryPoint;
    this->m_ShaderInputType     = shaderInputType;
}

CORE_ASSET_TYPE
ShaderAssetDescriptor::GetAssetType()
{
	return this->m_AssetType;
}

CORE_SHADER_INPUT_TYPE
ShaderAssetDescriptor::GetShaderInputType ()
{
    return this->m_ShaderInputType;
}


ShaderAssetDescriptorExtended::ShaderAssetDescriptorExtended(const string & name, const string & path, CORE_ID identifier) : ShaderAssetDescriptor(name, path, identifier)
{
    this->m_Name = name;
}