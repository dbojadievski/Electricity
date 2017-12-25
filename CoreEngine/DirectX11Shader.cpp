#pragma comment(lib, "D3DCompiler.lib")
#include <assert.h>
#include "DirectX11Shader.h"
//#include <d3d11.h>
#include <d3dcompiler.h>

DirectX11Shader::DirectX11Shader(CORE_STRING pStrVertexShader, CORE_STRING pStrFragmentShader, CORE_ID shaderId) : ShaderBase(pStrVertexShader, pStrFragmentShader)
{
	this->Compile();
	this->m_Identifier = shaderId;
}

DirectX11Shader::~DirectX11Shader()
{
	this->m_pVertexShader->Release();
	this->m_pPixelShader->Release();
}

CORE_BOOLEAN 
DirectX11Shader::Compile()
{
	CORE_BOOLEAN wasCompiled = true;

	HRESULT errCode;
	
	LPCWSTR pathShader = L"Assets\\Shaders\\shaders.shader";
	ID3DBlob * pErrMsg;

	errCode = D3DCompileFromFile(pathShader, NULL, NULL, "VShader", "vs_4_0", 0, 0, &this->m_pVertexShaderBlob, &pErrMsg);
	assert(!pErrMsg);
	if (errCode)
	{
		OutputDebugStringA((char*)pErrMsg->GetBufferPointer());
		pErrMsg->Release();
		wasCompiled = false;
		goto end;
	}

	errCode = D3DCompileFromFile(pathShader, NULL, NULL, "PShader", "ps_4_0", 0, 0, &this->m_pPixelShaderBlob, &pErrMsg);
	assert(!pErrMsg);
	if (errCode)
	{
		OutputDebugStringA((char*)pErrMsg->GetBufferPointer());
		pErrMsg->Release();
		wasCompiled = false;
		goto end;
	}

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

ID3D11VertexShader const *
DirectX11Shader::GetVertexShader()
{
	ID3D11VertexShader const * pRetVal = this->m_pVertexShader;
	return pRetVal;


}

ID3D11PixelShader const *
	DirectX11Shader::GetPixelShader()
{
	ID3D11PixelShader const * pRetVal = this->m_pPixelShader;
	return pRetVal;
}

CORE_ID
DirectX11Shader::GetIdentifier()
{
	return this->m_Identifier;
}