cbuffer cbPerObject
{
    float4x4 WVP;
};

Texture2D ObjTexture;
SamplerState ObjSamplerState;

struct VOut
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD;
};

VOut VShader(float4 position : POSITION, float2 texCoord : TEXCOORD)
{
	VOut output;

	output.position = mul(position, WVP);
	output.texCoord = texCoord;

	return output;
}


float4 PShader(float4 position : SV_POSITION, float2 texCoord : TEXCOORD) : SV_TARGET
{
	return ObjTexture.Sample(ObjSamplerState, texCoord);
}