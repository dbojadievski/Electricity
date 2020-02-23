#pragma once
#include "CoreHeaders.h"
#include "IEngineSystem.h"
#include "AssetDescriptor.h"
#include "MeshAssetDescriptor.h"
#include "TextureAssetDescriptor.h"
#include "MaterialDescriptor.h"
#include "ShaderDescriptor.h"
#include "Model.h"
#include "Mesh.h"

class IAssetManager : IEngineSystem
{
public:

    virtual CoreMesh				*	GetMesh (AssetDescriptor * pDescriptor) = 0;
	virtual MeshAssetDescriptor		*	GetMeshDescriptor(const string & name) = 0;
	virtual TextureAssetDescriptor	*	GetTextureDescriptor(const string & name) = 0;
	virtual ShaderAssetDescriptor	*	GetShaderDescriptor(const string & name) = 0;
	virtual MaterialAssetDescriptor *	GetMaterialDescriptor (const string & name) = 0;

	virtual ModelAssetDescriptor	*	GetModelDescriptor(const string & name) = 0;
	virtual ShaderPassDescriptor	*	GetShaderPassDescriptor(const string & name) = 0;

	virtual ModelAssetDescriptorExtended * GetModelData(const string & name) = 0;
};