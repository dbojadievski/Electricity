#pragma once
#include "AssetDescriptor.h"

#include <string>
using namespace std;

class ShaderAssetDescriptor : public AssetDescriptor
{
public:
	ShaderAssetDescriptor(const string & path, CORE_ID identifier);

	CORE_ASSET_TYPE GetAssetType();
};

class ShaderAssetDescriptorExtended : public ShaderAssetDescriptor
{

public:
	ShaderAssetDescriptorExtended(const string & path, CORE_ID identifier);
};