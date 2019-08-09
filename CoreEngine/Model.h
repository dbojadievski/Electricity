#pragma once

#include "TextureAssetDescriptor.h"
#include "ShaderDescriptor.h"
#include "MeshAssetDescriptor.h"
class ModelAssetDescriptor : public AssetDescriptor
{
private:
    vector<MeshAssetDescriptor *>		m_Meshes;
    vector<TextureAssetDescriptor *>	m_Textures;
	vector<ShaderAssetDescriptor *>		m_Shaders;

public:
    CORE_BOOLEAN HasMesh (MeshAssetDescriptor * pMeshDesc);
    CORE_BOOLEAN HasTexture (TextureAssetDescriptor * pTexDesc);

    CORE_BOOLEAN AddMesh (MeshAssetDescriptor * pMeshDesc);
    CORE_BOOLEAN AddTexture (TextureAssetDescriptor * pTexDesc);

	CORE_BOOLEAN HasShader( ShaderAssetDescriptor * pShaderDesc );
	CORE_BOOLEAN AddShader(ShaderAssetDescriptor * pShaderDesc);

    ModelAssetDescriptor (string & name, CORE_ID id);

    size_t NumMeshes ();
    MeshAssetDescriptor * GetMeshAt (size_t idx);

    size_t NumTextures ();
    TextureAssetDescriptor * GetTextureAt (size_t idx);

	size_t NumShaders();
	ShaderAssetDescriptor * GetShaderAt (size_t idx);
};

typedef ModelAssetDescriptor ModelDescriptor;