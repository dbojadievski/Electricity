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
Mesh::AddIndice(unsigned int indice1, unsigned int indice2)
{
	this->m_Indices.push_back(indice1);
	this->m_Indices.push_back(indice2);

	this->m_NumIndices += 2;
}

void
Mesh::AddIndice(unsigned int indice1, unsigned int indice2, unsigned int indice3)
{
	this->m_Indices.push_back(indice1);
	this->m_Indices.push_back(indice2);
	this->m_Indices.push_back(indice3);
		
	this->m_NumIndices += 3;
}
Mesh::Mesh(EMeshType type)
{
	this->m_NumIndices = 0;
	this->m_NumVertices = 0;

	this->m_Type = type;
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
	return (this->m_NumIndices * 2);
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

unsigned int *
Mesh::GetIndicesRaw()
{
	return this->m_Indices.data();
}