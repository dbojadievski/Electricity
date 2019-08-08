#pragma once

#include "TextureAssetDescriptor.h"
#include "ShaderDescriptor.h"
#include "MeshAssetDescriptor.h"
class ModelAssetDescriptor : AssetDescriptor
{
private:
    vector<MeshAssetDescriptor *>   m_Meshes;
    vector<TextureAssetDescriptor *> m_Textures;

public:
    CORE_BOOLEAN HasMesh (MeshAssetDescriptor * pMeshDesc);
    CORE_BOOLEAN HasTexture (TextureAssetDescriptor * pTexDesc);

    CORE_BOOLEAN AddMesh (MeshAssetDescriptor * pMeshDesc);
    CORE_BOOLEAN AddTexture (TextureAssetDescriptor * pTexDesc);

    ModelAssetDescriptor (string & name, CORE_ID id);

    size_t NumMeshes ();
    MeshAssetDescriptor * GetMeshAt (size_t idx);

    size_t NumTextures ();
    TextureAssetDescriptor * GetTextureAt (size_t idx);
};