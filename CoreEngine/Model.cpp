#include "Model.h"

CORE_BOOLEAN
ModelAssetDescriptor::HasMesh (MeshAssetDescriptor * pMeshDesc)
{
    assert (pMeshDesc);

    CORE_BOOLEAN hasMesh            = false;

	auto pMesh = this->m_pMesh;
	
	CORE_ID targetID	= pMeshDesc->GetIdentifier();

	if (pMesh)
	{
		CORE_ID currID		= pMesh->GetIdentifier();

		if (currID = targetID)
			hasMesh = true;
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
ModelAssetDescriptor::HasShader(ShaderAssetDescriptor * pShaderDesc)
{
	assert(pShaderDesc);

	CORE_BOOLEAN hasShader = false;
	for (auto it = this->m_Shaders.begin(); it != this->m_Shaders.end(); ++it)
	{
		ShaderAssetDescriptor * pDesc = *it;
		if (pDesc->GetIdentifier() == pShaderDesc->GetIdentifier())
		{
			hasShader = true;
			break;
		}
	}

	return hasShader;
}

CORE_BOOLEAN
ModelAssetDescriptor::HasShader(CORE_ID id)
{
	CORE_BOOLEAN hasShader	= false;
	
	size_t numShaders		= this->m_Shaders.size();
	for (size_t idx			= 0; idx < numShaders; idx++)
	{
		auto pSh			= this->GetShaderAt(idx);
		if (pSh->GetIdentifier() == id)
		{
			hasShader		= true;
			break;
		}
	}

	return hasShader;
}

CORE_BOOLEAN
ModelAssetDescriptor::HasPass(ShaderPassDescriptor * pPassDesc)
{
	assert(pPassDesc);

	CORE_BOOLEAN hasPass = false;
	for (auto it = this->m_Passes.begin(); it != this->m_Passes.end(); ++it)
	{
		ShaderPassDescriptor * pDesc = *it;
		if (pDesc->GetIdentifier() == pPassDesc->GetIdentifier())
		{
			hasPass = true;
			break;
		}
	}

	return hasPass;
}

CORE_BOOLEAN
ModelAssetDescriptor::AddMesh (MeshAssetDescriptor * pMeshDesc)
{
    assert (pMeshDesc);

    CORE_BOOLEAN isAdded = false;

    CORE_BOOLEAN alreadyAdded = this->HasMesh (pMeshDesc);
    if (!alreadyAdded)
    {
        this->m_pMesh = pMeshDesc;
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

CORE_BOOLEAN
ModelAssetDescriptor::AddShader(ShaderAssetDescriptor *pDesc)
{
	assert(pDesc);

	CORE_BOOLEAN isAdded = false;
	CORE_BOOLEAN alreadyAdded = this->HasShader(pDesc);
	if (!alreadyAdded)
	{
		this->m_Shaders.push_back(pDesc);
		isAdded = true;
	}

	return isAdded;
}

CORE_BOOLEAN
ModelAssetDescriptor::AddPass(ShaderPassDescriptor *pDesc)
{
	assert(pDesc);

	CORE_BOOLEAN isAdded = false;
	CORE_BOOLEAN alreadyAdded = this->HasPass(pDesc);
	if (!alreadyAdded)
	{
		this->m_Passes.push_back(pDesc);
		isAdded = true;
	}
	
	return isAdded;
}

size_t
ModelAssetDescriptor::NumMeshes ()
{
    return (this->m_pMesh != NULL);
}

size_t
ModelAssetDescriptor::NumTextures ()
{
    return this->m_Textures.size ();
}

size_t
ModelAssetDescriptor::NumShaders()
{
	size_t numShaders = this->m_Shaders.size();
	return numShaders;
}

size_t
ModelAssetDescriptor::NumPasses()
{
	size_t numPasses = this->m_Passes.size();
	return numPasses;
}

MeshAssetDescriptor *
ModelAssetDescriptor::GetMeshAt (size_t idx)
{
    assert (idx == 0);

    MeshAssetDescriptor * pRetVal = NULL;
    if (idx == 0)
        pRetVal = this->m_pMesh;
    
    return pRetVal;
}

TextureAssetDescriptor *
ModelAssetDescriptor::GetTextureAt (size_t idx)
{
    assert (idx < this->m_Textures.size ());

    TextureAssetDescriptor * pRetVal = NULL;
    size_t numTexes = this->m_Textures.size ();
    if (idx < numTexes)
        pRetVal = this->m_Textures[idx];

    return pRetVal;
}

ShaderAssetDescriptor *
ModelAssetDescriptor::GetShaderAt(size_t idx)
{
	assert(idx < this->m_Shaders.size());

	ShaderAssetDescriptor * pRetVal = NULL;
	size_t numShaders = this->NumShaders();
	if (idx < numShaders)
		pRetVal = this->m_Shaders[idx];

	return pRetVal;
}

ShaderPassDescriptor *
ModelAssetDescriptor::GetPassAt(size_t idx)
{
	assert(idx < this->m_Passes.size());

	ShaderPassDescriptor * pRetVal = NULL;
	size_t numPasses = this->NumPasses();
	if (idx < numPasses)
		pRetVal = this->m_Passes[idx];

	return pRetVal;
}


ModelAssetDescriptor::ModelAssetDescriptor (string & name, CORE_ID id)
{
	this->m_pMesh		= NULL;
    this->m_Name        = string(name);
    this->m_Identifier  = id;
	this->m_AssetType	= ASSET_TYPE_MODEL;
}



ModelAssetDescriptorExtended::ModelAssetDescriptorExtended(string & name, CORE_ID id)
{
	this->m_pMesh		= NULL;
	this->m_Name		= name;
	this->m_Identifier	= id;
	this->m_AssetType	= ASSET_TYPE_MODEL;

}

size_t
ModelAssetDescriptorExtended::NumMeshes() const
{
	return (this->m_pMesh != NULL);
}

size_t
ModelAssetDescriptorExtended::NumShaders() const
{
	size_t numShaders	= this->m_Shaders.size();
	return numShaders;
}

size_t
ModelAssetDescriptorExtended::NumTextures() const
{
	size_t numTextures = this->m_Textures.size();
	return numTextures;
}

const MeshAssetDescriptorExtended * const
ModelAssetDescriptorExtended::GetMeshAt(size_t idx) const
{
	MeshAssetDescriptorExtended * pMesh = NULL;
	if (idx == 0)
		pMesh	= this->m_pMesh;
	
	return pMesh;
}

const TextureAssetDescriptorExtended * const
ModelAssetDescriptorExtended::GetTextureAt(size_t idx) const
{
	TextureAssetDescriptorExtended * pTex = NULL;
	
	size_t numTexes = this->m_Textures.size();
	if (idx < numTexes)
		pTex		= this->m_Textures.at(idx);

	return pTex;
}

const ShaderAssetDescriptorExtended * const
ModelAssetDescriptorExtended::GetShaderAt(size_t idx) const
{
	ShaderAssetDescriptorExtended * pShader = NULL;

	size_t numShaders	= this->m_Shaders.size();
	if (idx < numShaders)
		pShader			= this->m_Shaders.at(idx);

	return pShader;
}

CORE_BOOLEAN
ModelAssetDescriptorExtended::HasShader(CORE_ID id) const
{
	CORE_BOOLEAN hasShader	= false;
	size_t numShaders		= this->m_Shaders.size();
	for (size_t idx			= 0; idx < numShaders; idx++)
	{
		auto pSh			= this->GetShaderAt(idx);
		if (pSh->GetIdentifier() == id)
		{
			hasShader		= true;
			break;
		}
	}

	return hasShader;
}

CORE_BOOLEAN
ModelAssetDescriptorExtended::AddMesh(MeshAssetDescriptorExtended * pMesh)
{
	assert(pMesh);
	CORE_BOOLEAN wasAdded = false;
	if (pMesh && !this->m_pMesh)
	{
		this->m_pMesh	= pMesh;
		wasAdded		= true;
	}

	return wasAdded;

}

CORE_BOOLEAN
ModelAssetDescriptorExtended::AddTexture(TextureAssetDescriptorExtended * pTex)
{
	assert(pTex);
	CORE_BOOLEAN wasAdded = false;

	if (pTex)
	{
		CORE_ID id			= pTex->GetIdentifier();
		size_t numTexes		= this->m_Textures.size();
		CORE_BOOLEAN exists = false;
		for (size_t currTex = 0; currTex < numTexes; currTex++)
		{
			auto at			= this->m_Textures[currTex];
			if (at->GetIdentifier() == id)
			{
				exists		= true;
				break;
			}
		}

		if (!exists)
		{
			this->m_Textures.push_back(pTex);
			wasAdded		= true;
		}
	}
	return wasAdded;
}

CORE_BOOLEAN
ModelAssetDescriptorExtended::AddShader(ShaderAssetDescriptorExtended * pShader)
{
	assert(pShader);
	CORE_BOOLEAN wasAdded = false;

	if (pShader)
	{
		CORE_ID id				= pShader->GetIdentifier();
		size_t numShaders		= this->m_Shaders.size();
		CORE_BOOLEAN exists		= false;
		for (size_t currShader	= 0; currShader < numShaders; currShader++)
		{
			auto at				= this->m_Shaders[currShader];
			if (at->GetIdentifier() == id)
			{
				exists			= true;
				break;
			}
		}

		if (!exists)
		{
			this->m_Shaders.push_back(pShader);
			wasAdded			= true;
		}
	}
	return wasAdded;
}

size_t
ModelAssetDescriptorExtended::NumPasses() const
{
	size_t numPasses = this->m_Passes.size();
	return numPasses;
}

const ShaderPassDescriptor * const
ModelAssetDescriptorExtended::GetPassAt(size_t idx) const
{
	return this->m_Passes.at(idx);
}

CORE_BOOLEAN
ModelAssetDescriptorExtended::AddPass(ShaderPassDescriptor * pDesc)
{
	this->m_Passes.push_back(pDesc);
	return true;
}

