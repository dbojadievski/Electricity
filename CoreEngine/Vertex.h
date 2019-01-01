#pragma once
#include "CoreTypes.h"
#include "Math.h"

enum CoreTextureChannels
{
    DIFFUSE         = 0,
    NUM_CHANNELS    = 8
};
struct Vertex
{
	VEC3  m_Position;
	VEC2 m_TextureCoordinates[NUM_CHANNELS];
	VEC3 m_Normal;

	Vertex();
	Vertex(const Vertex & original);
	Vertex(const VEC3 & position, const VEC2 & colour, const VEC3 & normal);
	Vertex(CORE_REAL x, CORE_REAL y, CORE_REAL z, CORE_REAL u, CORE_REAL v, CORE_REAL nX, CORE_REAL nY, CORE_REAL nZ);

	void SetPosition(const VEC3 & position);
	void SetTextureCoordinates(const VEC2 & textureCoordinates, size_t channel = 0);
	void SetNormal(const VEC3 & normal);

	size_t GetSize();
};