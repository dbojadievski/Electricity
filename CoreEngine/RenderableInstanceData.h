#pragma once

#include "CoreTypes.h"
#include "Math.h"

struct RenderableInstanceData
	// This really has no business being inside D3D code.
{
	FLOAT4 rowX;
	FLOAT4 rowY;
	FLOAT4 rowZ;
	FLOAT4 rowW;

	static size_t GetInstanceSize ();
};

inline size_t RenderableInstanceData::GetInstanceSize ()
{
	size_t size = 0;

	size += sizeof (rowX);
	size += sizeof (rowY);
	size += sizeof (rowZ);
	size += sizeof (rowW);

	return size;
}