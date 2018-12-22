#pragma once
#include "IAssetManager.h"
#include "IEventManager.h"
#include "ShaderDescriptor.h"
#include <map>
#include "CoreHeaders.h"

typedef map<CORE_ID, AssetDescriptor *> AssetMap;
typedef map<CORE_ID, ShaderAssetDescriptor *> ShaderMap;
typedef map<CORE_ID, ShaderAssetDescriptorExtended *> LoadedShaderMap;

class AssetManager : public IAssetManager
{
	IEventManager * m_pEventManager;
	
	map<CORE_ASSET_TYPE, CORE_ID> m_CurrentIdentifiers;
	ShaderMap m_ShaderMap;
	LoadedShaderMap m_LoadedShaderMap;

	CORE_ID GetNextIdentifier(const CORE_ASSET_TYPE assetType) const;
	CORE_ERROR LoadShader(ShaderAssetDescriptor * pShaderAsssetDescriptor);
public:

	AssetManager(IEventManager * pEVentManager);
	virtual CORE_ERROR IAssetManager::VRegisterShaders();

	virtual void IEngineSystem::Init(void);
	virtual void IEngineSystem::Update(CORE_DOUBLE dT);
	virtual void IEngineSystem::ShutDown();

	CORE_ERROR IsLoaded(__PARAM_IN__ AssetDescriptor * pAssetDescriptor, __PARAM_OUT__ CORE_BOOLEAN & retVal) const;
	CORE_ERROR LoadAsset(AssetDescriptor * pAssetDescriptor);
};