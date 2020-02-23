cbuffer cbPerObject // At slot 0.
{
	float4x4 WVP;
	float4x4 World;
};

cbuffer cbPerFrame // At slot 1.
{
	float4x4 CameraMatrix;
	float4x4 ProjectionMatrix;
	float4x4 ViewProjectionMatrix;
};

Texture2D ObjTexture;
SamplerState ObjSamplerState;

struct VS_INPUT
{
	float4 position: POSITION;
	float2 texCoord0: TEXCOORD0;
	float2 texCoord1: TEXCOORD1;
	float3 normal: NORMAL;

	float4 rowX: ROWX;
	float4 rowY: ROWY;
	float4 rowZ: ROWZ;
	float4 rowW: ROWW;
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

	float4x4 world		= (float4x4(input.rowX, input.rowY, input.rowZ, input.rowW));
	float4x4 wvp		= transpose (mul (ProjectionMatrix, mul (CameraMatrix, transpose (world))));
	output.position		= mul (input.position, wvp);

	output.texCoord = input.texCoord0;
	output.normal = mul(input.normal, world);

	return output;

}

float4 PShader(VOut input) : SV_TARGET
{
	input.normal = normalize(input.normal);
	float4 diffuse = ObjTexture.Sample(ObjSamplerState, input.texCoord);
	float3 finalColour = diffuse;

	return float4(finalColour.r, finalColour.g, finalColour.b, diffuse.a);
}