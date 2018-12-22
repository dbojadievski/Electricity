#pragma comment(lib, "D3DCompiler.lib")
#include <assert.h>
#include "DirectX11Shader.h"
//#include <d3d11.h>
#include <d3dcompiler.h>

DirectX11Shader::DirectX11Shader(LPWSTR pStrVertexShaderPath, LPCSTR pStrVertexShaderFunc, LPWSTR pStrFragmentShaderPath, LPCSTR pStrFragmentShaderFunc, CORE_ID shaderId)
{
	this->Compile(pStrVertexShaderPath, pStrVertexShaderFunc, pStrFragmentShaderPath, pStrFragmentShaderFunc);
	this->m_Identifier = shaderId;
	this->m_pInputLayout = NULL;
}

DirectX11Shader::~DirectX11Shader()
{
	this->m_pVertexShader->Release();
	this->m_pPixelShader->Release();
}

CORE_BOOLEAN 
DirectX11Shader::Compile(LPWSTR pStrVertexShaderPath, LPCSTR pStrVertexShaderFunc, LPWSTR pStrFragmentShaderPath, LPCSTR pStrFragmentShaderFunc)
{
	CORE_BOOLEAN wasCompiled = true;

	HRESULT errCode;
	
	ID3DBlob * pErrMsg;

	errCode = D3DCompileFromFile(pStrVertexShaderPath, NULL, NULL, pStrVertexShaderFunc, "vs_5_0", 0, 0, &this->m_pVertexShaderBlob, &pErrMsg);
	if (errCode)
	{
		OutputDebugStringA((char*)pErrMsg->GetBufferPointer());
		pErrMsg->Release();
		wasCompiled = false;
		goto end;
	}
	assert(errCode == S_OK);

	errCode = D3DCompileFromFile(pStrFragmentShaderPath, NULL, NULL, pStrFragmentShaderFunc, "ps_5_0", D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG, 0, &this->m_pPixelShaderBlob, &pErrMsg);
	if (errCode)
	{
		OutputDebugStringA((char*)pErrMsg->GetBufferPointer());
		pErrMsg->Release();
		wasCompiled = false;
		goto end;
	}
	assert(errCode == S_OK);

end:
	return wasCompiled;
}

DirectXShaderBufferDescriptor 
DirectX11Shader::GetVertexShaderBufferPointer()
{
	DirectXShaderBufferDescriptor buffDescriptor;
	
	buffDescriptor.m_pBuffer = this->m_pVertexShaderBlob->GetBufferPointer();
	buffDescriptor.m_size = this->m_pVertexShaderBlob->GetBufferSize();

	return buffDescriptor;
}

DirectXShaderBufferDescriptor
DirectX11Shader::GetPixelShaderBufferPointer()
{
	DirectXShaderBufferDescriptor buffDescriptor;

	buffDescriptor.m_pBuffer = this->m_pPixelShaderBlob->GetBufferPointer();
	buffDescriptor.m_size = this->m_pPixelShaderBlob->GetBufferSize();

	return buffDescriptor;
}

ID3D11VertexShader **
DirectX11Shader::GetVertexShader()
{
	return &this->m_pVertexShader;
}

ID3D11PixelShader **
	DirectX11Shader::GetPixelShader()
{
	return &this->m_pPixelShader;
}

CORE_ID
DirectX11Shader::GetIdentifier()
{
	return this->m_Identifier;
}

void
DirectX11Shader::SetInputLayout(ID3D11InputLayout * pLayout)
{
	assert(pLayout);
	this->m_pInputLayout = pLayout;
}

ID3D11InputLayout *
DirectX11Shader::GetInputLayout()
{
	return this->m_pInputLayout;
}



ID3D11Buffer *
DirectX11Shader::GetFrameConstantBuffer()
{
	return this->m_pFrameConstantBuffer->GetRawPointer();
}