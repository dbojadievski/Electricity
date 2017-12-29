#pragma once


#include "CoreTypes.h"
#include <d3d11.h>

#include <map>

class DirectXShaderBufferDescriptor
{
public:
	LPVOID m_pBuffer;
	size_t m_size;
};

class DirectX11Shader
{
	ID3D11VertexShader *m_pVertexShader;
	ID3D11PixelShader *m_pPixelShader;


	CORE_ID m_Identifier;
	ID3D10Blob * m_pVertexShaderBlob;
	ID3D10Blob * m_pPixelShaderBlob;

	CORE_BOOLEAN Compile();

public:
	DirectX11Shader(CORE_STRING pStrVertexShader, CORE_STRING pStrFragmentShader, CORE_ID shaderId);
	~DirectX11Shader();

	DirectXShaderBufferDescriptor GetVertexShaderBufferPointer();
	DirectXShaderBufferDescriptor GetPixelShaderBufferPointer();
	
	ID3D11VertexShader const * GetVertexShader();
	ID3D11PixelShader const * GetPixelShader();

	CORE_ID GetIdentifier();
};