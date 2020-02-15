#pragma once

#include <d3d11.h>

#include "CoreTypes.h"

#include "Mesh.h"

#include "DirectX11Buffer.h"
#include "DirectX11Shader.h"
#include "DirectX11Texture2D.h"

#include "Renderable.h"
#include "DirectX11RenderableInstance.h"
class DirectX11RenderableInstance;

typedef std::_Vector_iterator<std::_Vector_val<std::_Simple_types<DirectX11RenderableInstance *>>> DirectX11RenderableInstanceIterator;
class DirectX11Renderable : public Renderable
{

	friend class DirectX11Renderable;
private:
	Mesh *					m_pMesh;
	DirectX11Shader *		m_pShader;
	DirectX11Texture2D *	m_pTexture;
	

	DirectX11Buffer * m_pVertexBuffer;
	DirectX11Buffer * m_pInstanceBuffer;
	DirectX11Buffer * m_pIndexBuffer;
	D3D11_SUBRESOURCE_DATA m_InstanceData;
	D3D_PRIMITIVE_TOPOLOGY m_Topology;
	vector<DirectX11RenderableInstance *> m_pInstances;

public:

	DirectX11Renderable(Mesh * pMesh, DirectX11Texture2D * pTexture, DirectX11Shader * pShader, CORE_BOOLEAN isTransparent = false, D3D11_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	virtual ~DirectX11Renderable();

	void Buffer(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext);
	void ActivateBuffers(ID3D11DeviceContext * pDeviceContext);
	void DeactivateBuffers ();

	void Render(ID3D11DeviceContext * pDeviceContext);

	DirectX11RenderableInstance * Instantiate(CORE_ID id, FASTMAT4 transform, DirectX11RenderableInstance * pParent = NULL);
	CORE_BOOLEAN DeInstantiate (CORE_ID id);

	DWORD								GetInstanceCount();
	DirectX11RenderableInstanceIterator GetInstances();
	DirectX11RenderableInstanceIterator GetInstancesEnd();

	const DirectX11Shader * GetShader();
	const DirectX11Texture2D * GetTexture();
	const Mesh * GetMesh ();
};