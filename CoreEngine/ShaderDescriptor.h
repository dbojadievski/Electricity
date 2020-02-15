#pragma once

#include "ShaderType.h"
#include "AssetDescriptor.h"

#include <string>
using namespace std;

class ShaderAssetDescriptor : public AssetDescriptor
{
    string                  m_EntryPoint;
    CORE_SHADER_TYPE        m_ShaderType;
    CORE_SHADER_INPUT_TYPE  m_ShaderInputType;

public:
    ShaderAssetDescriptor (const string & name, const string & path, CORE_ID identifier);
    ShaderAssetDescriptor (const string & name, const string & path, CORE_ID identifier, CORE_SHADER_TYPE shaderType, CORE_SHADER_INPUT_TYPE shaderInputType, const string & entryPoint);

	CORE_ASSET_TYPE         GetAssetType();
    CORE_SHADER_TYPE        GetShaderType () const;
    CORE_SHADER_INPUT_TYPE  GetShaderInputType ();
    CORE_STRING             GetEntryPoint ();
};

class ShaderAssetDescriptorExtended : public ShaderAssetDescriptor
{

public:
	ShaderAssetDescriptorExtended(const string & name, const string & path, CORE_ID identifier);
};

class ShaderPassDescriptor : public AssetDescriptor
{
private:
	CORE_ID						m_VertexShader;
	CORE_ID						m_PixelShader;

public:
	ShaderPassDescriptor(const string & name, CORE_ID identifier, CORE_ID vertexShader, CORE_ID pixelShader);

	CORE_ID GetVertexShader() const;
	CORE_ID GetPixelShader() const;
};