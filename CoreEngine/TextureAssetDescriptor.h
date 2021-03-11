#pragma once
#include "AssetDescriptor.h"
#include "TextureType.h"
#include <string>

using std::string;
class TextureAssetDescriptor : public AssetDescriptor
{
protected:
    CORE_TEXTURE_TYPE       m_TextureType;
public:
    TextureAssetDescriptor (const string  & name, const string & path, CORE_ID identifier, CORE_TEXTURE_TYPE = TEXTURE_TYPE_2D);
    string GetPath ();

    CORE_TEXTURE_TYPE GetTextureType () const;
};

/*NOTE(Dino): Textures are only read from DDS files, which the renderer does on its own for now. So, we don't need to store the raw data of the texture, just the path.*/
typedef TextureAssetDescriptor TextureAssetDescriptorExtended;