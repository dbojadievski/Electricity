cbuffer cbPerObject
{
    float4x4 WVP;
	float4x4 World;
};

struct BasicLight
{
	float pad;
    float3 direction;
    float4 colourAmbient;
    float4 colourDiffuse;
};

cbuffer cbPerFrame
{
    BasicLight light;
};

Texture2D ObjTexture;
SamplerState ObjSamplerState;

struct VOut
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD;
	float3 normal: NORMAL;
};

VOut VShader(float4 position : POSITION, float2 texCoord : TEXCOORD, float3 normal : NORMAL)
{
	VOut output;

	output.position = mul(position, WVP);
	output.texCoord = texCoord;
	output.normal = mul(normal, World);
	return output;
}


float4 PShader(float4 position : SV_POSITION, float2 texCoord : TEXCOORD, float3 normal : NORMAL) : SV_TARGET
{
	normal = normalize(normal);
	
	float4 diffuse = ObjTexture.Sample(ObjSamplerState, texCoord);
	float3 finalColour = diffuse * light.colourAmbient;
	finalColour += saturate(dot(light.direction, normal) * light.colourDiffuse * diffuse);
	
	return float4(finalColour, diffuse.a);
}