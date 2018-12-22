#include "AssetManager.h"

#include "TextureAssetDescriptor.h"
#include <string>
#include <iostream>
#include <filesystem>

#include "FileReader.h"

#include "AssetEvents.h"

using namespace std;
namespace fs = std::experimental::filesystem;
using namespace fs;

#pragma region System methods.
void 
AssetManager::Init() 
{
	this->VRegisterShaders();
    this->VRegisterTextures ();
}

void
AssetManager::Update(CORE_DOUBLE dT) { }

void
AssetManager::ShutDown() 
{
	for (auto it = this->m_ShaderMap.begin(); it != this->m_ShaderMap.end(); it++)
	{
		delete it->second;
	}
	this->m_ShaderMap.clear();

	this->m_CurrentIdentifiers.clear();
}
#pragma endregion


CORE_ERROR
AssetManager::VRegisterTextures ()
{
    CORE_ERROR err_code         = CORE_ERROR::ERR_OK;

    CORE_ID currTexId = 1;
    std::string textureDirectoryPath = "Assets\\Textures\\";
    for (auto & p : fs::directory_iterator (textureDirectoryPath))
    {
        path texPath = p.path ();
        string asString = texPath.string ();
        TextureAssetDescriptor * pTexAssetDescriptor = new TextureAssetDescriptor (asString, currTexId);
        auto pair = make_pair (currTexId, pTexAssetDescriptor);
        this->m_TextureMap.insert (pair);
        currTexId++;
    }

    return err_code;

}
CORE_ERROR
AssetManager::VRegisterShaders()
{
	CORE_ERROR errCode = CORE_ERROR::ERR_OK;

	CORE_ID currentShaderId = 1;
	std::string shaderDirectoryPath = "Assets\\Shaders\\";
	for (auto & p : fs::directory_iterator(shaderDirectoryPath))
	{
		path shaderPath = p.path();
		string asString = shaderPath.string();
		ShaderAssetDescriptor * pShaderAssetDescriptor = new ShaderAssetDescriptor(asString, currentShaderId);
		auto pair = make_pair(currentShaderId, pShaderAssetDescriptor);
		this->m_ShaderMap.insert(pair);
		currentShaderId++;
	}

	return errCode;
}

AssetManager::AssetManager(IEventManager * pEventManager)
{
	assert(pEventManager);
	this->m_pEventManager = pEventManager;
}

CORE_ID
AssetManager::GetNextIdentifier(const CORE_ASSET_TYPE assetType) const
{
	CORE_ID retVal = 1;
	
	auto iter = this->m_CurrentIdentifiers.find(assetType);
	if (iter != this->m_CurrentIdentifiers.end())
		retVal = ( iter->second + 1 );

	return retVal;
}

CORE_ERROR
AssetManager::LoadAsset(AssetDescriptor * pAssetDescriptor)
{
	assert(pAssetDescriptor);

	CORE_ERROR errCode = CORE_ERROR::ERR_OK;
	
	if (!pAssetDescriptor)
	{
		errCode = CORE_ERROR::ERR_PARAM_INVALID;
		goto end;
	}

	CORE_ASSET_TYPE assetType = pAssetDescriptor->GetAssetType();
	switch (assetType)
	{
	case CORE_ASSET_TYPE::ASSET_TYPE_SHADER:
		this->LoadShader( (ShaderAssetDescriptor * )pAssetDescriptor);
		break;
        case CORE_ASSET_TYPE::ASSET_TYPE_TEXTURE:
            this->LoadTexture ((TextureAssetDescriptor *)pAssetDescriptor);
            break;
	default:
		assert(0);
		break;
	}

	end:
	return errCode;
}

CORE_ERROR
AssetManager::IsLoaded( __PARAM_IN__ AssetDescriptor * pAssetDescriptor, __PARAM_OUT__ CORE_BOOLEAN & retVal) const
{
	CORE_ERROR errCode = CORE_ERROR::ERR_OK;

	assert(pAssetDescriptor);
	if (!pAssetDescriptor)
		errCode = CORE_ERROR::ERR_PARAM_INVALID;
	else
	{
		CORE_ID identifier = pAssetDescriptor->GetIdentifier();
		CORE_ASSET_TYPE assetType = pAssetDescriptor->GetAssetType();
		switch (assetType)
		{
		    case CORE_ASSET_TYPE::ASSET_TYPE_SHADER:
		    {
			    auto itShader = this->m_LoadedShaderMap.find(identifier);
			    retVal = (itShader != this->m_LoadedShaderMap.end());
			    break;
		    }
            case CORE_ASSET_TYPE::ASSET_TYPE_TEXTURE:
            {
                auto itTex = this->m_LoadedTextureMap.find (identifier);
                retVal = (itTex != this->m_LoadedTextureMap.end ());
                break;
            }
		    default:
		    {
			    assert(false);
			    errCode = CORE_ERROR::ERR_PARAM_OUT_OF_RANGE;
		    }
		}
	}
	return errCode;

}

CORE_ERROR
AssetManager::LoadShader(ShaderAssetDescriptor * pShaderDescriptor)
{
	assert(pShaderDescriptor);

	CORE_ERROR errCode = CORE_ERROR::ERR_OK;

	if (!pShaderDescriptor)
		errCode = CORE_ERROR::ERR_PARAM_INVALID;
	else
	{
		CORE_ID identifier = pShaderDescriptor->GetIdentifier();
		CORE_BOOLEAN isShaderLoaded = false;
		CORE_ERROR errShaderLoaded = this->IsLoaded(pShaderDescriptor, isShaderLoaded);
		assert(errShaderLoaded == CORE_ERROR::ERR_OK);
		if (!isShaderLoaded)
		{
			string shaderPath = pShaderDescriptor->GetPath();
			string shaderSource = ReadFile(shaderPath);
			ShaderAssetDescriptorExtended * pEx = new ShaderAssetDescriptorExtended(shaderPath, identifier, shaderSource);
			auto pair = make_pair(identifier, pEx);
			this->m_LoadedShaderMap.insert(pair);

			AssetLoadedEventData * pAssetLoadedEventData = new AssetLoadedEventData(pShaderDescriptor);
			this->m_pEventManager->VQueueEvent(pAssetLoadedEventData);
		}
	}

	return errCode;
}

CORE_ERROR
AssetManager::LoadTexture (TextureAssetDescriptor * pTextureDescriptor)
{
    assert (pTextureDescriptor);

    CORE_ERROR errCode  = CORE_ERROR::ERR_OK;

    if (!pTextureDescriptor)
        errCode         = CORE_ERROR::ERR_PARAM_INVALID;
    else
    {
        CORE_ID identifier = pTextureDescriptor->GetIdentifier ();
        CORE_BOOLEAN isTexLoaded = false;
        CORE_ERROR errTexLoaded = this->IsLoaded (pTextureDescriptor, isTexLoaded);
        assert (errTexLoaded == CORE_ERROR::ERR_OK);
        if (!isTexLoaded)
        {
            string texPath = pTextureDescriptor->GetPath();
            string texData = ReadFile (texPath);

            AssetLoadedEventData * pAssetLoadedEventData = new AssetLoadedEventData (pTextureDescriptor);
            this->m_pEventManager->VQueueEvent (pAssetLoadedEventData);
        }
    }

    return errCode;
}