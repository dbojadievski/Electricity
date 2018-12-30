#pragma once
#include "IAssetManager.h"
#include "IEventManager.h"
#include "AssetEvents.h"
#include "ShaderDescriptor.h"
#include "TextureAssetDescriptor.h"
#include "MeshAssetDescriptor.h"
#include "Mesh.h"
#include <map>
#include "CoreHeaders.h"
#include "Model.h"
#include "tinyxml2.h"

using namespace tinyxml2;

typedef map<CORE_ASSET_TYPE, CORE_ID>            AssetIdMap;
typedef map<CORE_ID, AssetDescriptor *>          AssetMap;
typedef map<CORE_ID, ShaderAssetDescriptor *>    ShaderMap;
typedef map<CORE_ID, TextureAssetDescriptor *>   TextureMap;
typedef map<CORE_ID, MeshAssetDescriptor *>      MeshMap;
typedef map<CORE_ID, ModelAssetDescriptor *>     ModelMap;

typedef map<CORE_ID, CoreMesh *>                            LoadedMeshMap;
typedef map<CORE_ID, ShaderAssetDescriptorExtended *>       LoadedShaderMap;
typedef map<CORE_ID, TextureAssetDescriptor/*Extended*/ *>  LoadedTextureMap;

class AssetManager : public IAssetManager
{
	IEventManager *              m_pEventManager;
	
	ShaderMap           m_ShaderMap;
    TextureMap          m_TextureMap;
    MeshMap             m_MeshMap;
    ModelMap            m_ModelMap;
	AssetIdMap          m_CurrentIdentifiers;

    LoadedMeshMap       m_LoadedMeshes;
	LoadedShaderMap     m_LoadedShaderMap;
    LoadedTextureMap    m_LoadedTextureMap;
	
    CORE_ID GetNextIdentifier(const CORE_ASSET_TYPE assetType) const;
	CORE_ERROR LoadShader(ShaderAssetDescriptor * pShaderAsssetDescriptor);
    CORE_ERROR AssetManager::LoadTexture (TextureAssetDescriptor * pTextureDescriptor);
    CORE_ERROR LoadMesh (MeshAssetDescriptor * pMesh);

    /* Event handlers*/
    void OnAssetLoadFailed (AssetLoadFailedEventData *pEventData);
public:

	AssetManager(IEventManager * pEVentManager);
    virtual CORE_ERROR AssetManager::VRegisterTextures (XMLNode * pXmlTextureList);
    virtual CORE_ERROR AssetManager::VRegisterShaders (XMLNode * pXmlShaders);
    virtual CORE_ERROR AssetManager::VRegisterMeshes (XMLNode * pXmlMeshList);
    virtual CORE_ERROR AssetManager::VRegisterModels (XMLNode * pXmlModelList);

    virtual CoreMesh * AssetManager::GetMesh (AssetDescriptor * pDescriptor);

    MeshAssetDescriptor * GetMeshDescriptor (const string & name);
    TextureAssetDescriptor * GetTextureDescriptor (const string & name);
    ShaderAssetDescriptor * GetShaderDescriptor (const string & name);


    virtual void IEngineSystem::Init(void);
	virtual void IEngineSystem::Update(CORE_DOUBLE dT);
	virtual void IEngineSystem::ShutDown();

	CORE_ERROR IsLoaded(__PARAM_IN__ AssetDescriptor * pAssetDescriptor, __PARAM_OUT__ CORE_BOOLEAN & retVal) const;
	CORE_ERROR LoadAsset(AssetDescriptor * pAssetDescriptor);
    void UnloadShader (ShaderAssetDescriptor * pDesc);
    void UnloadTexture (TextureAssetDescriptor *pDesc);
    void UnloadMesh (MeshAssetDescriptor * pDesc);
};