#include "Model.h"

CORE_BOOLEAN
ModelAssetDescriptor::HasMesh (MeshAssetDescriptor * pMeshDesc)
{
    assert (pMeshDesc);

    CORE_BOOLEAN hasMesh            = false;
    for (auto it = this->m_Meshes.begin (); it != this->m_Meshes.end (); ++it)
    {
        MeshAssetDescriptor * pDesc = *it;
        if (pDesc->GetIdentifier () == pMeshDesc->GetIdentifier ())
        {
            hasMesh                 = true;
            break;
        }
    }

    return hasMesh;
}

CORE_BOOLEAN
ModelAssetDescriptor::HasTexture (TextureAssetDescriptor * pTexDesc)
{
    assert (pTexDesc);

    CORE_BOOLEAN hasTex = false;
    for (auto it = this->m_Textures.begin (); it != this->m_Textures.end (); ++it)
    {
        TextureAssetDescriptor * pDesc = *it;
        if (pDesc->GetIdentifier () == pTexDesc->GetIdentifier ())
        {
            hasTex = true;
            break;
        }
    }

    return hasTex;
}

CORE_BOOLEAN
ModelAssetDescriptor::AddMesh (MeshAssetDescriptor * pMeshDesc)
{
    assert (pMeshDesc);

    CORE_BOOLEAN isAdded = false;

    CORE_BOOLEAN alreadyAdded = this->HasMesh (pMeshDesc);
    if (!alreadyAdded)
    {
        this->m_Meshes.push_back (pMeshDesc);
        isAdded = true;
    }

    return isAdded;
}

CORE_BOOLEAN
ModelAssetDescriptor::AddTexture (TextureAssetDescriptor * pDesc)
{
    assert (pDesc);

    CORE_BOOLEAN isAdded = false;

    CORE_BOOLEAN alreadyAdded = this->HasTexture (pDesc);
    if (!alreadyAdded)
    {
        this->m_Textures.push_back (pDesc);
        isAdded = true;
    }

    return isAdded;
}

ModelAssetDescriptor::ModelAssetDescriptor (string & name, CORE_ID id)
{
    this->m_Name        = name;
    this->m_Identifier  = id;
}