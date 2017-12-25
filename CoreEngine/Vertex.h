#pragma once
#include "CoreTypes.h"
#include "Math.h"


struct Vertex
{
	VEC3  m_Position;
	VEC2 m_TextureCoordinates;

	Vertex();
	Vertex(const Vertex & original);
	Vertex(const VEC3 & position, const VEC2 & colour);
	Vertex(CORE_REAL x, CORE_REAL y, CORE_REAL z, CORE_REAL u, CORE_REAL v);

	void SetPosition(const VEC3 & position);
	void SetTextureCoordinates(const VEC2 & textureCoordinates);

	size_t GetSize();
};