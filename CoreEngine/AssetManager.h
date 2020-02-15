#pragma once
#include "tinyxml2.h"
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

using namespace tinyxml2;

typedef map<CORE_ASSET_TYPE, CORE_ID>            AssetIdMap;
typedef map<CORE_ID, AssetDescriptor *>          AssetMap;
typedef map<CORE_ID, ShaderAssetDescriptor *>    ShaderMap;
typedef map<CORE_ID, TextureAssetDescriptor *>   TextureMap;
typedef map<CORE_ID, MeshAssetDescriptor *>      MeshMap;
typedef map<CORE_ID, ModelAssetDescriptor *>     ModelMap;
typedef map<CORE_ID, ShaderPassDescriptor *>	 ShaderPassMap;

typedef map<CORE_ID, MeshAssetDescriptorExtended *>         LoadedMeshMap;
typedef map<CORE_ID, ShaderAssetDescriptorExtended *>       LoadedShaderMap;
typedef map<CORE_ID, TextureAssetDescriptorExtended *>		LoadedTextureMap;
typedef map<CORE_ID, ModelAssetDescriptorExtended *>		LoadedModelMap;
typedef map<CORE_ID, ShaderPassDescriptor *>				LoadedPassMap;
class AssetManager : public IAssetManager
{
	IEventManager *     m_pEventManager;
	
	ShaderMap           m_ShaderMap;
    TextureMap          m_TextureMap;
    MeshMap             m_MeshMap;
    ModelMap            m_ModelMap;
	ShaderPassMap		m_ShaderPassMap;

	AssetIdMap          m_CurrentIdentifiers;

    LoadedMeshMap       m_LoadedMeshes;
	LoadedShaderMap     m_LoadedShaderMap;
    LoadedTextureMap    m_LoadedTextureMap;
	LoadedModelMap		m_LoadedModelMap;
	LoadedPassMap		m_LoadedPassMap;
	
    CORE_ID GetNextIdentifier(const CORE_ASSET_TYPE assetType);
	CORE_ERROR LoadShader(ShaderAssetDescriptor * pShaderAsssetDescriptor);
    CORE_ERROR AssetManager::LoadTexture (TextureAssetDescriptor * pTextureDescriptor);
    CORE_ERROR LoadMesh (MeshAssetDescriptor * pMesh);
    CORE_ERROR LoadModel (ModelAssetDescriptor * pModelDesc);
	CORE_ERROR LoadPass(ShaderPassDescriptor * pPass);
	ShaderAssetDescriptor * GetShaderDescriptor(const string & name, const SHADER_TYPE type);

    /* Event handlers*/
    void OnAssetLoadFailed (AssetLoadFailedEventData *pEventData);
	void OnSceneLoaded(IEventData *pEventData);
	void OnEntityComponentAdded(IEventData *pEventData);
public:

	AssetManager(IEventManager * pEVentManager);
	void RegisterTexture (TextureAssetDescriptor * pTex);
	void RegisterMesh (MeshAssetDescriptor * pTex);
	void RegisterShader (ShaderAssetDescriptor * pTex);
	void RegisterPass(ShaderPassDescriptor * pPass);
	void RegisterModel (ModelAssetDescriptor * pTex);

	virtual CORE_ERROR AssetManager::VRegisterTextures (XMLNode * pXmlTextureList);
    virtual CORE_ERROR AssetManager::VRegisterShaders (XMLNode * pXmlShaders);
	virtual CORE_ERROR AssetManager::VRegisterPasses(XMLNode * pXmlPassList);
    virtual CORE_ERROR AssetManager::VRegisterMeshes (XMLNode * pXmlMeshList);
    virtual CORE_ERROR AssetManager::VRegisterModels (XMLNode * pXmlModelList);

    virtual CoreMesh * AssetManager::GetMesh (AssetDescriptor * pDescriptor);

	virtual MeshAssetDescriptor * GetMeshDescriptor(const string & name);
    virtual TextureAssetDescriptor * GetTextureDescriptor (const string & name);
    virtual ShaderAssetDescriptor * GetShaderDescriptor (const string & name);
	virtual ModelAssetDescriptor * GetModelDescriptor(const string & name);
	virtual ModelAssetDescriptorExtended * GetModelData(const string & name);
	virtual ShaderPassDescriptor * GetShaderPassDescriptor(const string & name);

    virtual void IEngineSystem::Init(void);
	virtual void IEngineSystem::Update(CORE_DOUBLE dT);
	virtual void IEngineSystem::ShutDown();

	CORE_ERROR IsLoaded(__PARAM_IN__ AssetDescriptor * pAssetDescriptor, __PARAM_OUT__ CORE_BOOLEAN & retVal) const;
	CORE_ERROR LoadAsset(AssetDescriptor * pAssetDescriptor);
    void UnloadShader (ShaderAssetDescriptor * pDesc);
    void UnloadTexture (TextureAssetDescriptor *pDesc);
    void UnloadMesh (MeshAssetDescriptor * pDesc);

	CORE_ERROR LoadMeshFromPath (const string * path, vector<MeshAssetDescriptorExtended *> * pImportedMeshes);
};