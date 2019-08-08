#include "AssetManager.h"

#include "AssetEvents.h"
#include "TextureAssetDescriptor.h"
#include "Model.h"

#include <string>
#include <iostream>
#include <filesystem>

#include "FileReader.h"
#include "AssetEvents.h"
#include "tinyxml2.h"
#include "ShaderTypeConverter.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "Mesh.h"

#include "SceneNode.h"
#include "Scene.h"

using namespace std;
namespace fs = std::experimental::filesystem;
using namespace fs;
using namespace tinyxml2;


#pragma region System methods.

void 
AssetManager::Init() 
{

    /* Parse asset list.*/
    XMLDocument doc;
    doc.LoadFile ("Assets\\assetList.xml");
    XMLElement * pXmlAssetList      = doc.FirstChildElement ("assets");
    
    XMLElement * pXmlShaderList     = pXmlAssetList->FirstChildElement ("shaders");
    if (pXmlShaderList->NoChildren ())
        assert (false);
    else
         this->VRegisterShaders (pXmlShaderList);

    CORE_ERROR loadedTextures           = ERR_OK;
    XMLElement * pXmlTextureList        = pXmlAssetList->FirstChildElement ("textures");
    if (!pXmlTextureList || pXmlTextureList->NoChildren ())
        assert (false);
    else
        this->VRegisterTextures(pXmlTextureList);

    XMLElement * pXmlMeshList = pXmlAssetList->FirstChildElement ( "meshes" );
    if (!pXmlMeshList || pXmlMeshList->NoChildren ())
        assert (false);
    else
        this->VRegisterMeshes (pXmlMeshList);

    XMLElement * pXmlModelList = pXmlAssetList->FirstChildElement ("models");
    if (!pXmlModelList || pXmlModelList->NoChildren ())
        assert (false);
    else
        this->VRegisterModels (pXmlModelList);

    this->LoadMesh ((this->m_MeshMap.at(2)));
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

    for (auto it = this->m_MeshMap.begin (); it != this->m_MeshMap.end (); it++)
    {
        delete it->second;
    }
    this->m_MeshMap.clear ();

    for (auto it = this->m_TextureMap.begin (); it != this->m_TextureMap.end (); it++)
    {
        delete it->second;
    }
    this->m_TextureMap.clear ();

    for (auto it = this->m_ModelMap.begin (); it != this->m_ModelMap.end (); it++)
    {
        delete it->second;
    }
    this->m_ModelMap.clear ();

	this->m_CurrentIdentifiers.clear();
}
#pragma endregion



CORE_ERROR
AssetManager::VRegisterShaders (XMLNode * pXmlShaderList)
{
    CORE_ERROR retVal       = ERR_OK;

    XMLNode * pShaderNode = pXmlShaderList->FirstChild ();
    while (pShaderNode)
    {
        string name, path, entryPoint;
        CORE_SHADER_TYPE type = SHADER_TYPE_UNUSED;
        CORE_SHADER_INPUT_TYPE inputType = SHADER_INPUT_UNUSED;

        XMLElement * pShaderEl = pShaderNode->ToElement ();
        assert (pShaderEl);

        const char * pStrName = pShaderEl->Attribute ("name");
        assert (pStrName);
        if (!pStrName)
            retVal = ERR_FAILED;
        else
            name = string (pStrName);

        const char * pStrPath = pShaderEl->Attribute ("path");
        assert (pStrPath);
        if (!pStrPath)
            retVal = ERR_FAILED;
        else
            path = string (pStrPath);

        const char * pStrEntry = pShaderEl->Attribute ("entryPoint");
        assert (pStrEntry);
        if (!pStrEntry)
            retVal = ERR_FAILED;
        else
            entryPoint = string (pStrEntry);

        const char * pStrType = pShaderEl->Attribute ("type");
        assert (pStrType);
        if (!pStrType)
            retVal = ERR_FAILED;
        else
            type = ConvertStringToShaderType (pStrType);

        const char * pStrInputType = pShaderEl->Attribute ("input");
        assert (pStrInputType);
        if (!pStrInputType)
            retVal = ERR_FAILED;
        else
            inputType = ConvertStringToShaderInputType (pStrInputType);


		CORE_ID shaderId = GetNextIdentifier(ASSET_TYPE_SHADER);
        ShaderAssetDescriptor * pDesc = new ShaderAssetDescriptor (name, path, shaderId, type, inputType, entryPoint);
        auto pair = make_pair (shaderId, pDesc);
        this->m_ShaderMap.insert (pair);

        pShaderNode = pShaderNode->NextSibling ();
    }

    return retVal;
}

void
AssetManager::RegisterTexture (TextureAssetDescriptor * pDesc)
{
	assert (pDesc);
	CORE_ID id = pDesc->GetIdentifier ();
	auto pair = make_pair (id, pDesc);
	this->m_TextureMap.insert (pair);
	this->m_CurrentIdentifiers.insert_or_assign (CORE_ASSET_TYPE::ASSET_TYPE_TEXTURE, id);
}

void
AssetManager::RegisterMesh (MeshAssetDescriptor * pMesh)
{
	assert (pMesh);
	CORE_ID id = pMesh->GetIdentifier ();
	auto pair = make_pair (id, pMesh);
	this->m_MeshMap.insert (pair);
	this->m_CurrentIdentifiers.insert_or_assign (ASSET_TYPE_MESH, id);
}

void
AssetManager::RegisterShader (ShaderAssetDescriptor * pDesc)
{
	assert (pDesc);
	CORE_ID id = pDesc->GetIdentifier ();
	auto pair = make_pair (id, pDesc);
	this->m_ShaderMap.insert (pair);
	this->m_CurrentIdentifiers.insert_or_assign (CORE_ASSET_TYPE::ASSET_TYPE_SHADER, id);
}

void
AssetManager::RegisterModel (ModelAssetDescriptor * pDesc)
{
	assert (pDesc);
	CORE_ID id = pDesc->GetIdentifier ();
	auto pair = make_pair (id, pDesc);
	this->m_ModelMap.insert (pair);
	this->m_CurrentIdentifiers.insert_or_assign (CORE_ASSET_TYPE::ASSET_TYPE_MODEL, id);
}


CORE_ERROR
AssetManager::VRegisterTextures (XMLNode * pXmlTextureList)
{
    CORE_ERROR loadedTextures = ERR_OK;

    if (!pXmlTextureList || pXmlTextureList->NoChildren ())
        assert (false);
    else
    {
        XMLNode * pTexNode = pXmlTextureList->FirstChild ();
        while (pTexNode)
        {
            string name, path;
            CORE_TEXTURE_TYPE texType = TEXTURE_TYPE_UNUSED;

            XMLElement * pTexEl = pTexNode->ToElement ();

            const char * pStrName = pTexEl->Attribute ("name");
            if (!pStrName)
                loadedTextures = ERR_FAILED;
            else
                name = string (pStrName);

            const char * pStrPath = pTexEl->Attribute ("path");
            if (!pStrPath)
                loadedTextures = ERR_FAILED;
            else
                path = string (pStrPath);

            const char * pStrType = pTexEl->Attribute ("type");
            if (!pStrType)
                loadedTextures = ERR_FAILED;
            else
                texType = ConvertStringToTextureType (pStrType);

            if (loadedTextures == ERR_OK)
            {
				CORE_ID id = this->GetNextIdentifier(ASSET_TYPE_TEXTURE);
                TextureAssetDescriptor * pTexDesc = new TextureAssetDescriptor (name, path, id, texType);
				this->RegisterTexture (pTexDesc);


                pTexNode = pTexNode->NextSibling ();
            }
        }
    }

    return loadedTextures;
}

CORE_ERROR
AssetManager::VRegisterMeshes (XMLNode * pXmlMeshesList)
{
    CORE_ERROR loadedMeshes = ERR_OK;

    assert (pXmlMeshesList);

    if (pXmlMeshesList && !pXmlMeshesList->NoChildren ())
    {
        XMLNode * pMeshNode = pXmlMeshesList->FirstChild ();
        while (pMeshNode)
        {
			CORE_ID id = this->GetNextIdentifier(ASSET_TYPE_MESH);
            string name, path;
            
            XMLElement * pMeshEl    = pMeshNode->ToElement ();

            const char * pStrName   = pMeshEl->Attribute ("name");
            if (!pStrName)
                loadedMeshes        = ERR_FAILED;
            else
                name                = string (pStrName);

            const char * pStrPath   = pMeshEl->Attribute ("path");
            if (!pStrPath)
                loadedMeshes        = ERR_FAILED;
            else
                path                = string (pStrPath);

            if (loadedMeshes == ERR_OK)
            {
                MeshAssetDescriptor * pDesc = new MeshAssetDescriptor (name, path, id);
				this->RegisterMesh (pDesc);

                pMeshNode = pMeshNode->NextSibling ();
            }
        }
    }

    return loadedMeshes;
}

CORE_ERROR
AssetManager::VRegisterModels (XMLNode * pXmlModelsList)
{
    CORE_ERROR retVal = ERR_OK;

    assert (pXmlModelsList);
    if (pXmlModelsList && !pXmlModelsList->NoChildren ())
    {
        CORE_ID id = 1;
        XMLNode * pModelNode = pXmlModelsList->FirstChild ();
        while (pModelNode)
        {

            string name;
            XMLElement *pElem = pModelNode->ToElement ();

            const char * pStrName = pElem->Attribute ("name");
            if (!pStrName)
                retVal = ERR_FAILED;
            
            ModelAssetDescriptor * pModelDesc = new ModelAssetDescriptor (name, id);
            
            /*NOTE(Dino): Next, we process the meshes and textures inside the model. */
            XMLNode * pChild = pElem->FirstChild ();
            while (pChild)
            {
                auto text = pChild->Value ();
                if (!strncmp(text, "mesh", 4))
                {
                    auto meshName = pChild->ToElement ()->Attribute("name");
                    MeshAssetDescriptor * pMesh = this->GetMeshDescriptor (meshName);
                    pModelDesc->AddMesh (pMesh);
                }
                else if ( !strncmp(text, "texture", 7))
                {
                    auto texName = pChild->ToElement ()->Attribute ("name");
                    TextureAssetDescriptor * pTex = this->GetTextureDescriptor (texName);
                    pModelDesc->AddTexture (pTex);
                }
                pChild = pChild->NextSibling ();
            }

            auto pair   = make_pair (id, pModelDesc);
            this->m_ModelMap.insert (pair);
            pModelNode  = pModelNode->NextSibling ();
        }
    }

    return retVal;
}

AssetManager::AssetManager(IEventManager * pEventManager)
{
	assert(pEventManager);
	this->m_pEventManager = pEventManager;
}

CORE_ID
AssetManager::GetNextIdentifier(const CORE_ASSET_TYPE assetType)
{
	CORE_ID retVal = 1;
	
	auto iter = this->m_CurrentIdentifiers.find(assetType);
	if (iter != this->m_CurrentIdentifiers.end ())
		retVal = (iter->second + 1);
	
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
        case CORE_ASSET_TYPE::ASSET_TYPE_MESH:
            this->LoadMesh ((MeshAssetDescriptor *)pAssetDescriptor);
            break;
        case CORE_ASSET_TYPE::ASSET_TYPE_MODEL:
            this->LoadModel ((ModelAssetDescriptor *)pAssetDescriptor);
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
            case CORE_ASSET_TYPE::ASSET_TYPE_MESH:
            {
                auto itMesh = this->m_LoadedMeshes.find (identifier);
                retVal      = (itMesh != this->m_LoadedMeshes.end ());
                break;
            }
            case CORE_ASSET_TYPE::ASSET_TYPE_MODEL:
            /* NOTE(Dino): A model is loaded if and only if every sub-asset is loaded. */
            {
                CORE_BOOLEAN isModelLoaded            = true;
                ModelAssetDescriptor * pModelDesc   = (ModelAssetDescriptor *) pAssetDescriptor;

                size_t numMeshes    = pModelDesc->NumMeshes ();
                for (size_t meshIdx = 0; meshIdx < numMeshes; meshIdx++)
                {
                    MeshAssetDescriptor * pMeshDesc = pModelDesc->GetMeshAt (meshIdx);
                    CORE_BOOLEAN isLoaded           = false;
                    CORE_ERROR err                  = this->IsLoaded (pMeshDesc, isLoaded);
                    if (err != ERR_OK || !isLoaded)
                    {
                        isModelLoaded  = false;
                        break;
                    }
                }

                if (isModelLoaded)
                {
                    size_t numTexes = pModelDesc->NumTextures ();
                    for (size_t texIdx = 0; texIdx < numTexes; texIdx++)
                    {
                        TextureAssetDescriptor * pTexDesc = pModelDesc->GetTextureAt (texIdx);
                        CORE_BOOLEAN isLoaded               = false;
                        CORE_ERROR err = this->IsLoaded (pTexDesc, isLoaded);
                        if (err != ERR_OK || !isLoaded)
                        {
                            isModelLoaded = false;
                            break;
                        }
                    }
                }

                retVal = isModelLoaded;
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
            string name = pShaderDescriptor->GetName ();
			string shaderPath = pShaderDescriptor->GetPath();
			string shaderSource = ReadFile(shaderPath);
			ShaderAssetDescriptorExtended * pEx = new ShaderAssetDescriptorExtended(name, shaderPath, identifier);
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

CORE_ERROR
AssetManager::LoadMesh (MeshAssetDescriptor * pMeshDescriptor)
{
    assert (pMeshDescriptor);

    CORE_ERROR errCode      = CORE_ERROR::ERR_OK;

    if (!pMeshDescriptor)
        goto end;
    else
    {
        CORE_ID identifier = pMeshDescriptor->GetIdentifier ();
        CORE_BOOLEAN isMeshLoaded = false;
        CORE_ERROR errMeshLoaded = this->IsLoaded (pMeshDescriptor, isMeshLoaded);
        assert (errMeshLoaded == CORE_ERROR::ERR_OK);
        if (!isMeshLoaded)
        {
            string meshPath = pMeshDescriptor->GetPath ();
            vector<CoreMesh *> loadedMeshes;
            LoadMeshFromPath (&meshPath, &loadedMeshes);
            for (auto it = loadedMeshes.begin (); it != loadedMeshes.end (); ++it)
            {
                CoreMesh * pMesh = *it;
                assert (pMesh);
				pMesh->m_Name = pMeshDescriptor->GetName();
                auto pair = make_pair (identifier, pMesh);
                this->m_LoadedMeshes.insert (pair);
            }

            AssetLoadedEventData * pAssetLoadedEventData = new AssetLoadedEventData (pMeshDescriptor);
            this->m_pEventManager->VQueueEvent (pAssetLoadedEventData);
        }
    }

end:
    return errCode;
}

CORE_ERROR
AssetManager::LoadModel (ModelAssetDescriptor * pModelDesc)
{
    assert (pModelDesc);

    CORE_ERROR retVal   = ERR_OK;
    if (!pModelDesc)
        retVal          = ERR_PARAM_INVALID;
    else
    {
        size_t numMeshes    = pModelDesc->NumMeshes ();
        for (size_t meshIdx = 0; meshIdx < numMeshes; meshIdx++)
        {
            MeshAssetDescriptor * pMesh = pModelDesc->GetMeshAt (meshIdx);
            retVal                      = this->LoadMesh (pMesh);
        }

        size_t numTexes         = pModelDesc->NumTextures ();
        for (size_t texIdx      = 0; texIdx < numTexes; texIdx++)
        {
            TextureAssetDescriptor * pTex   = pModelDesc->GetTextureAt (texIdx);
            retVal                          = this->LoadTexture (pTex);
        }
    }

    return retVal;
}

void
AssetManager::OnAssetLoadFailed (AssetLoadFailedEventData *pEventData)
{
    assert (pEventData);

    switch (pEventData->GetAssetType ())
    {
        case ASSET_TYPE_TEXTURE:
            this->UnloadTexture (this->m_LoadedTextureMap.at (pEventData->m_pAssetDescriptorExtended->GetIdentifier ()));
            break;
        case ASSET_TYPE_SHADER:
            this->UnloadShader(this->m_LoadedShaderMap.at(pEventData->m_pAssetDescriptorExtended->GetIdentifier()));
            break;
        case ASSET_TYPE_MESH:
            //this->UnloadMesh (this->m_LoadedMeshes.at (pEventData->m_pAssetDescriptorExtended->GetIdentifier ()));
            break;
        default:
            assert (false);
            break;
    }
}

void 
AssetManager::UnloadShader (ShaderAssetDescriptor * pDesc)
{
    CORE_ID identifier  = pDesc->GetIdentifier ();
    this->m_LoadedShaderMap.erase (identifier);
    delete pDesc;
}

void
AssetManager::UnloadTexture (TextureAssetDescriptor * pDesc)
{
    CORE_ID identifier  = pDesc->GetIdentifier ();
    this->m_LoadedTextureMap.erase (identifier);
    delete pDesc;
}

void
AssetManager::UnloadMesh (MeshAssetDescriptor * pMesh)
{
    assert (pMesh);

    CORE_ID identifier = pMesh->GetIdentifier ();
    this->m_LoadedMeshes.erase (identifier);
    delete pMesh;
}

CORE_ERROR 
AssetManager::LoadMeshFromPath (const string * path, vector<CoreMesh *> * pImportedMeshes)
{
    CORE_ERROR errCode  = ERR_FAILED;

    assert (path);
    assert (pImportedMeshes);

    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile (path->c_str(),
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_SortByPType);

    if (scene)
    {
		CORE_ID id = this->GetNextIdentifier (CORE_ASSET_TYPE::ASSET_TYPE_MESH);
        CoreMesh * pParentMesh      = new CoreMesh (id);

        // here we process the scene.
        for (size_t i = 0; i < scene->mNumMeshes; i++)
        {
            aiMesh * pMesh = scene->mMeshes[i];
            if (pMesh)
            {
                CoreMesh * pCoreMesh = new CoreMesh (GetNextIdentifier(ASSET_TYPE_MESH));

                if (pMesh->mNumVertices > 0)
                    for (size_t currVertex = 0; currVertex < pMesh->mNumVertices; currVertex++)
                    {
                        Vertex * pCoreVertex = new Vertex ();
                        pCoreVertex->m_Position.x = pMesh->mVertices[currVertex].x;
                        pCoreVertex->m_Position.y = pMesh->mVertices[currVertex].y;
                        pCoreVertex->m_Position.z = pMesh->mVertices[currVertex].z;

                        for (size_t uvIdx = 0; uvIdx < CoreTextureChannels::NUM_CHANNELS; uvIdx++)
                        if (pMesh->mNumUVComponents[uvIdx] >= 2)
                        {
                            aiVector3D * pTexCoords     = pMesh->mTextureCoords[uvIdx];
                            aiVector3D * pCoords        = &pTexCoords[currVertex];
                            pCoreVertex->m_TextureCoordinates[uvIdx].x = pCoords->x;
                            pCoreVertex->m_TextureCoordinates[uvIdx].y = pCoords->y;
                        }
                        
                        if (pMesh->HasNormals ())
                        {
                            aiVector3D * pNormal = pMesh->mNormals;
                            pCoreVertex->m_Normal.x = pNormal[currVertex].x;
                            pCoreVertex->m_Normal.y = pNormal[currVertex].y;
                            pCoreVertex->m_Normal.z = pNormal[currVertex].z;
                        }

                        pCoreMesh->AddVertex (pCoreVertex);
                    }
                    
                if (pMesh->HasNormals())
                    for (size_t currIdx = 0; currIdx < pMesh->mNormals->Length (); currIdx++)
                    {
                        aiVector3D normal = pMesh->mNormals[currIdx];
                        VEC3 * pCoreNormal = new VEC3 (normal.x, normal.y, normal.z);
                        pCoreMesh->AddNormal (pCoreNormal);
                    }
                
                if (pMesh->HasFaces())
                    for (size_t faceIdx = 0; faceIdx < pMesh->mNumFaces; faceIdx++)
                    {
                        aiFace * pFace = &pMesh->mFaces[faceIdx];
                        size_t numIndices = pFace->mNumIndices;

                        for (size_t currIndice = 0; currIndice < pFace->mNumIndices; currIndice++)
                            pCoreMesh->AddIndice (pFace->mIndices[currIndice]);
                        numIndices++;
                    }

                pParentMesh->AddSubMesh (pCoreMesh);
            }
        }

        if (pParentMesh->GetSubMeshCount () == 1)
        {
            auto child      = pParentMesh->GetSubMeshAt (0);
            pParentMesh->EraseSubMeshAt (0);
            delete pParentMesh;
            pParentMesh     = child;
        }

        pImportedMeshes->push_back (pParentMesh);
        errCode     = ERR_OK;

    }
    

end:
    return errCode;
}

CoreMesh *
AssetManager::GetMesh (AssetDescriptor * pDescriptor)
{
    CoreMesh * pRetVal      = NULL;
    CORE_BOOLEAN isLoaded   = false;
    
    CORE_ERROR err          = this->IsLoaded (pDescriptor, isLoaded);
    assert (err == ERR_OK);
    if (err == ERR_OK && isLoaded)
    {
        for (auto it = this->m_LoadedMeshes.begin (); it != this->m_LoadedMeshes.end (); ++it)
        {
            if (it->first == pDescriptor->GetIdentifier ())
            {
                pRetVal = it->second;
                break;
            }
        }
    }

    return pRetVal;
}

MeshAssetDescriptor *
AssetManager::GetMeshDescriptor (const string & name)
{
    MeshAssetDescriptor * retVal = NULL;

    for (auto it = this->m_MeshMap.begin (); it != this->m_MeshMap.end (); ++it)
    {
        MeshAssetDescriptor * pDesc = it->second;
        assert (pDesc);
        if (!name.compare (pDesc->GetName ()))
        {
            retVal = pDesc;
            break;
        }
    }
    return retVal;
}

TextureAssetDescriptor *
AssetManager::GetTextureDescriptor (const string & name)
{
    TextureAssetDescriptor * retVal = NULL;

    for (auto it = this->m_TextureMap.begin (); it != this->m_TextureMap.end (); ++it)
    {
        TextureAssetDescriptor * pDesc = it->second;
        assert (pDesc);
        if (!name.compare (pDesc->GetName ()))
        {
            retVal = pDesc;
            break;
        }
    }
    return retVal;
}

ShaderAssetDescriptor *
AssetManager::GetShaderDescriptor (const string & name)
{
    ShaderAssetDescriptor * retVal = NULL;

    for (auto it = this->m_ShaderMap.begin (); it != this->m_ShaderMap.end (); ++it)
    {
        ShaderAssetDescriptor * pDesc = it->second;
        assert (pDesc);
        if (!name.compare (pDesc->GetName ()))
        {
            retVal = pDesc;
            break;
        }
    }
    return retVal;
}