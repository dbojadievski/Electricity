#pragma once
#include "CoreTypes.h"
#include "Vertex.h"
#include "Math.h"

#include <vector>

using namespace std;

enum EMeshType
{
	MESH_TYPE_TRIANGLE,
	MESH_TYPE_TRIANGLE_STRIPE
};

struct Mesh
{
private:
	size_t m_NumIndices;
	size_t m_NumVertices;
	
	vector<unsigned long int> m_Indices;
	vector<Vertex> m_Vertices;

	EMeshType m_Type;

public:
	void AddIndice(unsigned long int indice);
	void AddIndice(unsigned long int indice1, unsigned long int indice2);
	void AddIndice(unsigned long int indice1, unsigned long int indice2, unsigned long int indice3);
	
	void AddVertex(Vertex * pVertex);

	size_t GetIndiceCount();
	size_t GetVertexCount();

	static size_t GetVertexSize();
	size_t GetVertexArraySize();


	size_t GetIndexArraySize();

	Mesh(EMeshType type = EMeshType::MESH_TYPE_TRIANGLE);
	~Mesh();

	Vertex * GetVerticesRaw();
	unsigned long int * GetIndicesRaw();
};