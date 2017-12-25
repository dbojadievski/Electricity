#pragma once

#include <DirectXMath.h>

using namespace DirectX;

typedef XMFLOAT2 FLOAT2;
typedef XMFLOAT3 FLOAT3;
typedef XMFLOAT4 FLOAT4;

typedef FLOAT2 VEC2;
typedef FLOAT3 VEC3;
typedef FLOAT4 VEC4;

typedef XMINT2 INT2;
typedef XMINT3 INT3;
typedef XMINT4 INT4;


typedef DirectX::XMMATRIX FASTMAT4;
typedef DirectX::XMVECTOR FASTVEC;

#define FASTVEC_SET XMVectorSet
#define FASTMAT_IDENTITY XMMatrixIdentity
#define FASTMAT_TRANSPOSE XMMatrixTranspose

typedef DirectX::XMFLOAT3X3 MAT3;
typedef DirectX::XMFLOAT4X4 MAT4;