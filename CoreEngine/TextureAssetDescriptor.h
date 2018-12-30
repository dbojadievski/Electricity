#pragma once
#include "AssetDescriptor.h"
#include "TextureType.h"
#include <string>
using namespace std;

class TextureAssetDescriptor : public AssetDescriptor
{
    CORE_TEXTURE_TYPE       m_TextureType;
public:
    TextureAssetDescriptor (const string  & name, const string & path, CORE_ID identifier, CORE_TEXTURE_TYPE = TEXTURE_TYPE_2D);
    string GetPath ();

    CORE_TEXTURE_TYPE GetTextureType () const;
};