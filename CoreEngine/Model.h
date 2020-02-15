#pragma once

#include "TextureAssetDescriptor.h"
#include "ShaderDescriptor.h"
#include "MeshAssetDescriptor.h"
class ModelAssetDescriptor : public AssetDescriptor
{
private:
    MeshAssetDescriptor *				m_pMesh;
    vector<TextureAssetDescriptor *>	m_Textures;
	vector<ShaderAssetDescriptor *>		m_Shaders;
	vector<ShaderPassDescriptor *>		m_Passes;


public:
    CORE_BOOLEAN HasMesh (MeshAssetDescriptor * pMeshDesc);
    CORE_BOOLEAN AddMesh (MeshAssetDescriptor * pMeshDesc);

    CORE_BOOLEAN AddTexture (TextureAssetDescriptor * pTexDesc);
    CORE_BOOLEAN HasTexture (TextureAssetDescriptor * pTexDesc);

	CORE_BOOLEAN HasShader( ShaderAssetDescriptor * pShaderDesc );
	CORE_BOOLEAN AddShader(ShaderAssetDescriptor * pShaderDesc);

	CORE_BOOLEAN HasPass(ShaderPassDescriptor * pPassDesc);
	CORE_BOOLEAN AddPass(ShaderPassDescriptor * pPassDesc);

    ModelAssetDescriptor (string & name, CORE_ID id);

    size_t NumMeshes ();
    MeshAssetDescriptor * GetMeshAt (size_t idx);

    size_t NumTextures ();
    TextureAssetDescriptor * GetTextureAt (size_t idx);

	size_t NumShaders();
	ShaderAssetDescriptor * GetShaderAt (size_t idx);
	CORE_BOOLEAN HasShader(CORE_ID id);

	size_t NumPasses();
	ShaderPassDescriptor * GetPassAt(size_t idx);
};

class ModelAssetDescriptorExtended : public AssetDescriptor
{
private:
	MeshAssetDescriptorExtended *				m_pMesh;
	vector<TextureAssetDescriptorExtended *>	m_Textures;
	vector<ShaderAssetDescriptorExtended *>		m_Shaders;
	vector<ShaderPassDescriptor *>				m_Passes;

public:
	ModelAssetDescriptorExtended(string & name, CORE_ID id);
	
	size_t NumMeshes() const;
	const MeshAssetDescriptorExtended * const GetMeshAt(size_t idx) const;
	CORE_BOOLEAN AddMesh(MeshAssetDescriptorExtended * pMesh);

	size_t NumTextures() const;
	const TextureAssetDescriptorExtended * const GetTextureAt(size_t idx) const;
	CORE_BOOLEAN AddTexture(TextureAssetDescriptorExtended * pTex);

	size_t NumShaders() const;
	const ShaderAssetDescriptorExtended * const GetShaderAt(size_t idx) const;
	CORE_BOOLEAN AddShader(ShaderAssetDescriptorExtended * pShader);
	CORE_BOOLEAN HasShader(CORE_ID id) const;

	size_t NumPasses() const;
	const ShaderPassDescriptor * const GetPassAt(size_t idx) const;
	CORE_BOOLEAN AddPass(ShaderPassDescriptor * pPassDesc);
};

typedef ModelAssetDescriptor			ModelDescriptor;
typedef ModelAssetDescriptorExtended	ModelDescriptorExtended;