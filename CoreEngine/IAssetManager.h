#pragma once
#include "CoreHeaders.h"
#include "IEngineSystem.h"
#include "AssetDescriptor.h"
#include "MeshAssetDescriptor.h"
#include "TextureAssetDescriptor.h"
#include "ShaderDescriptor.h"
#include "Mesh.h"

class IAssetManager : IEngineSystem
{
public:

    virtual CoreMesh				*	GetMesh (AssetDescriptor * pDescriptor) = 0;
	virtual MeshAssetDescriptor		*	GetMeshDescriptor(const string & name) = 0;
	virtual TextureAssetDescriptor	*	GetTextureDescriptor(const string & name) = 0;
	virtual ShaderAssetDescriptor	*	GetShaderDescriptor(const string & name) = 0;
};