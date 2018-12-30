#include "Mesh.h"

void 
Mesh::AddVertex(Vertex * pVertex)
{
	if (pVertex)
	{
		this->m_Vertices.push_back(*pVertex);
		this->m_NumVertices++;
	}
}

void 
Mesh::AddIndice(unsigned long int indice1, unsigned long int indice2)
{
	this->m_Indices.push_back(indice1);
	this->m_Indices.push_back(indice2);

	this->m_NumIndices += 2;
}

void
Mesh::AddIndice(unsigned long int indice1, unsigned long int indice2, unsigned long int indice3)
{
	this->m_Indices.push_back(indice1);
	this->m_Indices.push_back(indice2);
	this->m_Indices.push_back(indice3);
		
	this->m_NumIndices += 3;
}

void
Mesh::AddIndice(unsigned long int indice)
{
	this->m_Indices.push_back(indice);
	this->m_NumIndices++;
}

void
Mesh::AddNormal (VEC3 * pNormal)
{
    this->m_Normals.push_back (*pNormal);
    this->m_NumNormals++;
}

void
Mesh::AddSubMesh (const Mesh * pSubMesh)
{
    assert (pSubMesh);
    if (!pSubMesh)  
        goto end;

    this->m_SubMeshes.push_back ( (Mesh *) pSubMesh);

    end:;
}

Mesh *
Mesh::GetSubMeshAt (const size_t idx)
{
    Mesh * pMesh        = NULL;
    
    size_t numSubMeshes = this->m_SubMeshes.size ();
    if (numSubMeshes > idx)
        pMesh = this->m_SubMeshes[idx];

    return pMesh;
}

CORE_ERROR
Mesh::EraseSubMeshAt (const size_t idx)
{
    CORE_ERROR retVal       = ERR_OK;

    size_t numSubMeshes     = this->m_SubMeshes.size ();
    if (numSubMeshes <= idx)
    {
        retVal              = ERR_PARAM_OUT_OF_RANGE;
        goto end;
    }

    this->m_SubMeshes.erase (this->m_SubMeshes.begin() + idx);

    end:
    return retVal;
}

Mesh::Mesh(EMeshType type)
{
	this->m_NumIndices  = 0;
	this->m_NumVertices = 0;
    this->m_NumNormals  = 0;

	this->m_Type        = type;
}

Mesh::~Mesh()
{
}

size_t
Mesh::GetVertexCount()
{
	return this->m_NumVertices;
}

size_t
Mesh::GetIndiceCount()
{
	return (this->m_NumIndices);
}

size_t
Mesh::GetNormalCount ()
{
    return (this->m_NumNormals);
}

size_t
Mesh::GetSubMeshCount ()
{
    size_t size     = this->m_SubMeshes.size ();
    return size;
}
size_t
Mesh::GetVertexArraySize()
{
	size_t size = (sizeof(Vertex) * this->m_NumVertices);
	return size;
}

size_t
Mesh::GetIndexArraySize()
{
	size_t size = (sizeof(CORE_DWORD) *  this->m_NumIndices);
	return size;
}

Vertex *
Mesh::GetVerticesRaw()
{
	return this->m_Vertices.data();
}

size_t Mesh::GetVertexSize()
{
	return sizeof(Vertex);
}

unsigned long int *
Mesh::GetIndicesRaw()
{
	return this->m_Indices.data();
}