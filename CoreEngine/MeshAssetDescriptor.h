#pragma once

#include "AssetDescriptor.h"
#include <string>

#include "Mesh.h"

using std::string;

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
		this->m_Name = name;
		this->m_Path = path;
		this->m_Identifier = identifier;
    }

	void SetName(const string & name)
	{
		this->m_Name = string(name.c_str());
	}

	CoreMesh * GetMesh() const
	{
		return m_pMesh;
	}

	const string & GetName() const;
};