#include "MeshAssetDescriptor.h"

MeshAssetDescriptor::MeshAssetDescriptor (const string & name, const string & path, CORE_ID identifier)
{    
    this->m_AssetType           = ASSET_TYPE_MESH;
    this->m_Path                = path;
    this->m_Identifier          = identifier;
    this->m_Name                = name;
}

MeshAssetDescriptor::~MeshAssetDescriptor ()
{
}