cbuffer cbPerObject
{
    float4x4 WVP;
	float4x4 World;
};


struct VOut
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD;
	float3 normal: NORMAL;
};

VOut VShader(float4 position : POSITION, float2 texCoord : TEXCOORD, float3 normal : NORMAL, vec3: IPOS, vec3: IROT, vec3: ISCALE)
{
	VOut output;

	output.position = mul(position, WVP);
	output.texCoord = texCoord;
	output.normal = mul(normal, World);
	return output;
}


float4 PShader(float4 position : SV_POSITION, float2 texCoord : TEXCOORD, float3 normal : NORMAL) : SV_TARGET
{	
	return float4(0.0, 1.0, 0.0, 1.0);
}