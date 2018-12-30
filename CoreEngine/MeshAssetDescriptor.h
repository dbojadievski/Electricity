#pragma once

#include "AssetDescriptor.h"
#include <string>

#include "Mesh.h"
using namespace std;

class MeshAssetDescriptor : public AssetDescriptor
{
public:
    MeshAssetDescriptor (const string & name, const string & path, CORE_ID identifier);
    ~MeshAssetDescriptor ();
};

class MeshAssetDescriptorExtended : public MeshAssetDescriptor
{
private:
    CoreMesh * m_pMesh;
public:
    MeshAssetDescriptorExtended (const string & name, const string & path, CORE_ID identifier, CoreMesh * pMesh) : MeshAssetDescriptor (name, path, identifier)
    {
        this->m_pMesh = pMesh;
    }
};