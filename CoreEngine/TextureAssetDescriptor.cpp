#include "TextureAssetDescriptor.h"

TextureAssetDescriptor::TextureAssetDescriptor (const string & path, CORE_ID identifier)
{
    this->m_AssetType = CORE_ASSET_TYPE::ASSET_TYPE_TEXTURE;

    this->m_Path        = path;
    this->m_Identifier  = identifier;
}

string
TextureAssetDescriptor::GetPath ()
{
    return this->m_Path;
}