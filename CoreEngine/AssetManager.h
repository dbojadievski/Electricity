#pragma once
#include "IAssetManager.h"
#include "IEventManager.h"
#include "AssetEvents.h"
#include "ShaderDescriptor.h"
#include "TextureAssetDescriptor.h"
#include <map>
#include "CoreHeaders.h"

typedef map<CORE_ID, AssetDescriptor *> AssetMap;
typedef map<CORE_ID, ShaderAssetDescriptor *> ShaderMap;
typedef map<CORE_ID, TextureAssetDescriptor *> TextureMap;

typedef map<CORE_ID, AssetDescriptor *> AssetMap;
typedef map<CORE_ID, ShaderAssetDescriptorExtended *> LoadedShaderMap;
typedef map<CORE_ID, TextureAssetDescriptor/*Extended*/ *> LoadedTextureMap;

class AssetManager : public IAssetManager
{
	IEventManager * m_pEventManager;
	
	map<CORE_ASSET_TYPE, CORE_ID> m_CurrentIdentifiers;
	ShaderMap m_ShaderMap;
    TextureMap m_TextureMap;
	LoadedShaderMap m_LoadedShaderMap;
    LoadedTextureMap m_LoadedTextureMap;

	CORE_ID GetNextIdentifier(const CORE_ASSET_TYPE assetType) const;
	CORE_ERROR LoadShader(ShaderAssetDescriptor * pShaderAsssetDescriptor);
    CORE_ERROR AssetManager::LoadTexture (TextureAssetDescriptor * pTextureDescriptor);

    /* Event handlers*/
    void OnAssetLoadFailed (AssetLoadFailedEventData *pEventData);
public:

	AssetManager(IEventManager * pEVentManager);
	virtual CORE_ERROR IAssetManager::VRegisterShaders();
    virtual CORE_ERROR AssetManager::VRegisterTextures ();
	
    virtual void IEngineSystem::Init(void);
	virtual void IEngineSystem::Update(CORE_DOUBLE dT);
	virtual void IEngineSystem::ShutDown();

	CORE_ERROR IsLoaded(__PARAM_IN__ AssetDescriptor * pAssetDescriptor, __PARAM_OUT__ CORE_BOOLEAN & retVal) const;
	CORE_ERROR LoadAsset(AssetDescriptor * pAssetDescriptor);
    void UnloadShader (ShaderAssetDescriptor * pDesc);
    void UnloadTexture (TextureAssetDescriptor *pDesc);
};