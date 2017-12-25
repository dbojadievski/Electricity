#pragma once
#include <assimp/cimport.h>        // Plain-C interface
#include <assimp/scene.h>
#include <assimp/postprocess.h>     // Post processing flags

#include <assert.h>
#include "CoreTypes.h"


#include "Mesh.h"
#include "Vertex.h"

Mesh *
ProcessMesh(aiMesh * pMesh, const aiScene * pScene)
{
	Mesh * pCoreMesh = new Mesh();

	CORE_BOOLEAN hasNormals = pMesh->HasNormals();
	CORE_BOOLEAN hasTexCoords = pMesh->HasTextureCoords(0);
	CORE_BOOLEAN hasFaces = pMesh->HasFaces();

	size_t numUvChannels = pMesh->GetNumUVChannels();
	size_t numColorChannels = pMesh->GetNumColorChannels();

	for (size_t currVertexIdx = 0; currVertexIdx < pMesh->mNumVertices; currVertexIdx++)
	{
		Vertex  * pVertex = new Vertex();

		aiVector3D vertex = pMesh->mVertices[currVertexIdx];
		pVertex->m_Position.x = vertex.x;
		pVertex->m_Position.y = vertex.y;
		pVertex->m_Position.z = vertex.z;

		if (numColorChannels)
		{
			aiColor4D color = pMesh->mColors[0][currVertexIdx];
		}

		pCoreMesh->AddVertex(pVertex);
	}

	return pCoreMesh;
}

void
ProcessNode(aiNode * pNode, const aiScene *pScene, vector<Mesh *> * pMeshes)
{
	if (pNode && pScene)
	{
		for (size_t currMeshIdx = 0; currMeshIdx < pNode->mNumMeshes; currMeshIdx++)
		{
			aiMesh * pMesh = pScene->mMeshes[pNode->mMeshes[currMeshIdx]];
			Mesh * pCoreMesh = ProcessMesh(pMesh, pScene);
			pMeshes->push_back(pCoreMesh);
		}

		for (size_t currChildIdx = 0; currChildIdx < pNode->mNumChildren; currChildIdx++)
		{
			aiNode * pChildNode = pNode->mChildren[currChildIdx];
			ProcessNode(pChildNode, pScene, pMeshes);
		}
	}
}

CORE_BOOLEAN
ImportAsset(const std::string * pFile)
{
	CORE_BOOLEAN wasImported = false;
	
	vector<Mesh *> * pMeshes = new vector<Mesh *>();

	const char * pPath = pFile->c_str();
	const aiScene * pScene = aiImportFile(pPath, aiProcess_CalcTangentSpace | aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);

	if (pScene)
	{
		aiNode * pRootNode = pScene->mRootNode;
		ProcessNode(pRootNode, pScene, pMeshes);

		wasImported = true;
		aiReleaseImport(pScene);
	}
	else
	{
		/* TODO(Dino): We need a logging mechanism. */
		const char * pErr = aiGetErrorString();
		wasImported = false;
	}

	return wasImported;
}
