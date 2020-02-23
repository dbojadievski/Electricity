#pragma once

#include "CoreTypes.h"
#include "Math.h"

struct MaterialInstanceData
{
	FLOAT4		ambient;
	FLOAT4		diffuse;
	FLOAT4		specular;
	CORE_REAL	specularPower;
	FLOAT3		__padding__;

	static UINT GetSize ()
	{
		UINT size = 0;

		size += sizeof (ambient);
		size += sizeof (diffuse);
		
		size += sizeof (specular);
		size += sizeof (specularPower);
		
		size += sizeof (__padding__);

		return size;
	}
};
struct RenderableInstanceData
	// This really has no business being inside D3D code.
{
#pragma region Transformation matrix
	FLOAT4 rowX;
	FLOAT4 rowY;
	FLOAT4 rowZ;
	FLOAT4 rowW;
#pragma endregion

#pragma region material
	MaterialInstanceData material;
#pragma endregion
	static size_t GetInstanceSize ();
};

inline size_t RenderableInstanceData::GetInstanceSize ()
{
	size_t size = 0;

	// Size-up the transform. Remember to transpose it in the shader!
	size += sizeof (rowX);
	size += sizeof (rowY);
	size += sizeof (rowZ);
	size += sizeof (rowW);

	// Size-up the material. This doesn't need any modification there.
	size += sizeof (material);

	return size;
}