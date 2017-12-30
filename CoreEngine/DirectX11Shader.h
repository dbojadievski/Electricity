#pragma once


#include "CoreTypes.h"
#include <d3d11.h>
#include "DirectX11Buffer.h"
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

	CORE_BOOLEAN Compile(LPWSTR pStrVertexShaderPath, LPCSTR pStrVertexShaderFunc, LPWSTR pStrFragmentShaderPath, LPCSTR pStrFragmentShaderFunc);

	ID3D11InputLayout * m_pInputLayout;

	DirectX11Buffer * m_pFrameConstantBuffer;
	DirectX11Buffer * m_pFrameConstantStructuredBuffer;

	ID3D11Buffer * GetFrameConstantBuffer();
public:
	DirectX11Shader(LPWSTR pStrVertexShaderPath, LPCSTR pStrVertexShaderFunc, LPWSTR pStrFragmentShaderPath, LPCSTR pStrFragmentShaderFunc, CORE_ID shaderId);
	~DirectX11Shader();

	DirectXShaderBufferDescriptor GetVertexShaderBufferPointer();
	DirectXShaderBufferDescriptor GetPixelShaderBufferPointer();
	
	ID3D11VertexShader * GetVertexShader();
	ID3D11PixelShader * GetPixelShader();

	CORE_ID GetIdentifier();

	ID3D11InputLayout * GetInputLayout();
	void SetInputLayout(ID3D11InputLayout * pLayout);
};