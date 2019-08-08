#pragma once
#include "CoreTypes.h"
#include "Mesh.h"

class Renderable
{

protected:
	CORE_ID			m_RenderableID;
	Mesh *			m_pMesh;
	CORE_BOOLEAN	m_IsTransparent;
public:
	Renderable(Mesh * pMesh, CORE_BOOLEAN isTransparent);
	virtual ~Renderable();

	CORE_BOOLEAN IsTransparent() const;
	Mesh * GetMesh()const;
};

