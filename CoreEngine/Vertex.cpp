#include "Vertex.h"

Vertex::Vertex () : Vertex (0, 0, 0, 0, 0, 0, 0, 0)
{
    for (size_t idx = 0; idx < CoreTextureChannels::NUM_CHANNELS; idx++)
    {
        this->m_TextureCoordinates[idx].x = 0;
        this->m_TextureCoordinates[idx].y = 0;
    }
}

Vertex::Vertex(const Vertex & original)
{
	this->SetPosition(original.m_Position);
    for (size_t i = 0; i < 8; i++)
	    this->SetTextureCoordinates(original.m_TextureCoordinates[i], i);
}

Vertex::Vertex(const VEC3 & position, const VEC2 & textureCoordinates, const VEC3& normal)
{
	this->SetPosition(position);
	this->SetTextureCoordinates(textureCoordinates);
	this->SetNormal(normal);
}

Vertex::Vertex(CORE_REAL x, CORE_REAL y, CORE_REAL z, CORE_REAL u, CORE_REAL v, CORE_REAL nX, CORE_REAL nY, CORE_REAL nZ)
{
	VEC3 position(x, y, z);
	VEC2 texCoords(u, v);
	VEC3 normal(nX, nY, nZ);

	this->SetPosition(position);
	this->SetTextureCoordinates(texCoords);
	this->SetNormal(normal);
}


void 
Vertex::SetTextureCoordinates(const VEC2 & texCoords, size_t channel)
{
	this->m_TextureCoordinates[channel].x = texCoords.x;
	this->m_TextureCoordinates[channel].y = texCoords.y;
}

void
Vertex::SetPosition(const VEC3 & position)
{
	this->m_Position.x = position.x;
	this->m_Position.y = position.y;
	this->m_Position.z = position.z;
}

void
Vertex::SetNormal(const VEC3 & normal)
{
	this->m_Normal.x = normal.x;
	this->m_Normal.y = normal.y;
	this->m_Normal.z = normal.z;
}

size_t 
Vertex::GetSize()
{
	size_t size = 0;

	size += sizeof(this->m_Position);
	size += sizeof(this->m_TextureCoordinates);
	size += sizeof(this->m_Normal);

	return size;
}