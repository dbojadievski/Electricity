#pragma once
#include "CoreTypes.h"
#include "Vertex.h"
#include "Math.h"
#include "CoreError.h"
#include <vector>
#include <string>

using std::vector;
using std::string;
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
    size_t m_NumNormals;

	vector<unsigned long int> m_Indices;
	vector<Vertex> m_Vertices;
    vector<VEC3> m_Normals;

	EMeshType m_Type;

    Mesh * m_pParentMesh;
    vector<Mesh *> m_SubMeshes;

	CORE_ID m_MeshID;
public:
	string m_Name;
	void AddIndice(unsigned long int indice);
	void AddIndice(unsigned long int indice1, unsigned long int indice2);
	void AddIndice(unsigned long int indice1, unsigned long int indice2, unsigned long int indice3);
	
	void AddVertex(Vertex * pVertex);

    void AddNormal (VEC3 * pNormal);

    void AddSubMesh (const Mesh * pSubMesh);
    Mesh * GetSubMeshAt (const size_t idx);
    CORE_ERROR EraseSubMeshAt (const size_t idx);

	size_t GetIndiceCount();
	size_t GetVertexCount();
    size_t GetNormalCount ();

    size_t GetSubMeshCount ();

	static size_t GetVertexSize();
	size_t GetVertexArraySize();
    

	size_t GetIndexArraySize();
	CORE_ID GetIdentifier ();
	Mesh(CORE_ID meshID, EMeshType type = EMeshType::MESH_TYPE_TRIANGLE);
	~Mesh();

	Vertex * GetVerticesRaw();
	unsigned long int * GetIndicesRaw();
};

typedef Mesh CoreMesh;