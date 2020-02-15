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

CORE_SHADER_TYPE
ShaderAssetDescriptor::GetShaderType()
{
	return this->m_ShaderType;
}

CORE_STRING
ShaderAssetDescriptor::GetEntryPoint()
{
	return this->m_EntryPoint.c_str();
}

ShaderAssetDescriptorExtended::ShaderAssetDescriptorExtended(const string & name, const string & path, CORE_ID identifier) : ShaderAssetDescriptor(name, path, identifier)
{
    this->m_Name = name;
}

ShaderPassDescriptor::ShaderPassDescriptor(const string & name, CORE_ID id, CORE_ID vertexID, CORE_ID pixelID)
{
	this->m_Name			= name;
	this->m_Identifier		= id;

	this->m_VertexShader	= vertexID;
	this->m_PixelShader		= pixelID;

	this->m_AssetType		= ASSET_TYPE_PASS;
}

CORE_ID
ShaderPassDescriptor::GetVertexShader() const
{
	return this->m_VertexShader;
}

CORE_ID
ShaderPassDescriptor::GetPixelShader() const
{
	return this->m_PixelShader;
}