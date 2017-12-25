#include "Mesh.h"

class IRenderable
{
public:
};

class RenderableBase
{
private:
	Mesh * m_pMesh;
	size_t m_ID;

public:

	RenderableBase(Mesh * pMesh, size_t renderableID);
	~RenderableBase();

	const Mesh const * GetMesh();
	size_t GetID();

	virtual void Draw() = 0;
};