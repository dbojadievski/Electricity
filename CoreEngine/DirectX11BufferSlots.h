#pragma once
#include "CoreTypes.h"

typedef enum DirectX11VertexBufferSlots
{
	VERT_BUFF_SLOT_BASE_DATA		= 0,
	VERT_BUFF_SLOT_INSTANCE_DATA	= 1
} DirectX11VertexBufferSlots;

typedef enum DirectX11VertexShaderSlots
{
	VERT_SHADER_SLOT_INSTANCE_MATRICES = 0,
	VERT_SHADER_SLOT_FRAME_MATRICES		= 1
} DirectX11VertexShaderSlots;

typedef enum DirectX11PixelShaderSlots
{
	PIX_SHADER_SLOT_LIGHT_UNIFORM	= 0
};

typedef enum DirectX11TextureSlots
{
	TEX_SLOT_DIFFUSE				= 0
};