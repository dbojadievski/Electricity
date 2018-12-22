#pragma once
#include "AssetDescriptor.h"

#include <string>
using namespace std;

class TextureAssetDescriptor : public AssetDescriptor
{

public:
    TextureAssetDescriptor (const string & path, CORE_ID identifier);
    string GetPath ();
};