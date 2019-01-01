cbuffer cbPerObject
{
    float4x4 WVP;
	float4x4 World;
};

struct DirectionalLight
{
    float4 colourAmbient;
    float4 colourDiffuse;
	float pad;
    float3 direction;
};

cbuffer cbPerFrame
{
    DirectionalLight light;
};

StructuredBuffer<DirectionalLight> lightBuffer;

Texture2D ObjTexture;
SamplerState ObjSamplerState;

struct VS_INPUT
{
	float4 position : POSITION;
	float2 texCoord0 : TEXCOORD0;
	float2 texCoord1 : TEXCOORD1;
	float3 normal: NORMAL;
};

struct VOut
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD;
	float3 normal: NORMAL;
};

VOut VShader(VS_INPUT input)
{
	VOut output;

	output.position 	= mul(input.position, WVP);
	output.texCoord 	= input.texCoord0;
	output.normal 		= mul(input.normal, World);
	return output;
}


float4 PShader(VOut input) : SV_TARGET
{
	input.normal 		= normalize(input.normal);
	float4 diffuse 		= ObjTexture.Sample(ObjSamplerState, input.texCoord);
	float3 finalColour 	= diffuse;
	float3 ambient 		= diffuse;
	float3 saturation 	= float3(0, 0, 0);
	float4 outColour 	= float4(0.0, 0.0, 0.0, 1.0);
	uint numLights;
	uint stride;
	lightBuffer.GetDimensions(numLights, stride);
	
	finalColour 		= diffuse * light.colourAmbient;
	finalColour 		+= saturate(dot(light.direction, input.normal) * light.colourDiffuse * diffuse);
	
	return float4(finalColour.r, finalColour.g, finalColour.b, diffuse.a);
}