struct DirectionalLight
{
	float3 direction;
	float3 colour;
	float2 padding;
};

struct HemisphericLight
{
	float3 ambientDown;
	float3 ambientUp;
	float2 padding;
};

cbuffer cbPerFrame :register(b0)// At slot 1. used in the vertex shader.
{
	float4x4 CameraMatrix;
	float4x4 ProjectionMatrix;
	float4x4 ViewProjectionMatrix;
};

cbuffer DirectionalLightBuffer // at slot 3. Used in the pixel shader.
{
	DirectionalLight directionalLight;
}

cbuffer HemisphericLightBuffer
{
	HemisphericLight hemisphericLight;
};

StructuredBuffer<DirectionalLight> lightBuffer;
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

	float4 matAmbient:			MATERIALAMBIENT;
	float4 matDiffuse:			MATERIALDIFFUSE;
	float4 matSpecular:			MATERIALSPECULAR;
	float matSpecularPower:		MATERIALSPECULARPOWER;
	float3 matPadding:			MATERIALPADDING;
};


struct VOut
{
	float4 position : SV_POSITION;
	float2 texCoord : TEXCOORD;
	float3 normal: NORMAL;
};

VOut VShader (VS_INPUT input)
{
	VOut output;

	float4x4 world		= (float4x4(input.rowX, input.rowY, input.rowZ, input.rowW));
	float4x4 wvp		= transpose (mul (ProjectionMatrix, mul (CameraMatrix, transpose (world))));	
	output.position		= mul (input.position, wvp);
	
	output.texCoord		= input.texCoord0;
	output.normal		= mul (input.normal, world);

	return output;

}

float3 CalcAmbient (float3 normal, float3 colour)
{
	float3 up			= normal.y * 0.5 + 0.5; // Convert from [-1, 1] to [0, 1],
	float3 Ambient		= hemisphericLight.ambientDown + up * hemisphericLight.ambientUp;

	float3 result		= Ambient * colour;
	return result;
}

float4 PShader (VOut input) : SV_TARGET
{
	input.normal		= normalize (input.normal);
	float4 diffuse		= ObjTexture.Sample (ObjSamplerState, input.texCoord);
	float3 finalColour	= (diffuse.rgb * diffuse.rgb);

	uint numLights;
	uint stride;
	finalColour			+= saturate (dot (directionalLight.direction, input.normal) * directionalLight.colour * diffuse);
	lightBuffer.GetDimensions (numLights, stride);


	return float4(finalColour.r, finalColour.g, finalColour.b, diffuse.a);
}