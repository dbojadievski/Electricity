#pragma once
#include "CoreTypes.h"
#include "Mesh.h"

class Renderable
{

protected:
	CORE_ID			m_RenderableID;
	CORE_BOOLEAN	m_IsTransparent;

	CORE_ID m_MeshID;
	CORE_ID m_TextureID;
	CORE_ID m_ShaderID;

public:
	Renderable(CORE_ID meshID, CORE_ID textureID, CORE_ID shaderID, CORE_BOOLEAN isTransparent);
	virtual ~Renderable();

	CORE_BOOLEAN IsTransparent() const;
};

