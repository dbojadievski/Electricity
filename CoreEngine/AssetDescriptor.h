#pragma once
#include "CoreHeaders.h"
#include <string>
using namespace std;

enum CORE_ASSET_TYPE
{
	ASSET_TYPE_UNUSED = 0,
	ASSET_TYPE_SHADER,
	ASSET_TYPE_MESH,
	ASSET_TYPE_TEXTURE,
    ASSET_TYPE_MODEL,
	ASSET_TYPE_ANIMATION,
	ASSET_TYPE_SOUND,
	ASSET_TYPE_VIDEO
};
class AssetDescriptor
{
protected:
	CORE_ID m_Identifier;
	CORE_ASSET_TYPE m_AssetType;
	string m_Path;
    string m_Name;
public:
    string GetName ();
	string GetPath();
	CORE_ID GetIdentifier();
	CORE_ASSET_TYPE GetAssetType();
};