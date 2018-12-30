#include "TextureAssetDescriptor.h"

TextureAssetDescriptor::TextureAssetDescriptor (const string & name, const string & path, CORE_ID identifier, CORE_TEXTURE_TYPE type)
{
    this->m_AssetType = CORE_ASSET_TYPE::ASSET_TYPE_TEXTURE;

    this->m_Name        = name;
    this->m_Path        = path;
    this->m_Identifier  = identifier;
    this->m_TextureType = type;
}

string
TextureAssetDescriptor::GetPath ()
{
    return this->m_Path;
}

CORE_TEXTURE_TYPE 
TextureAssetDescriptor::GetTextureType () const
{
    return this->m_TextureType;
}