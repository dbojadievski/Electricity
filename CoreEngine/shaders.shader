struct VOut
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

XMMatrix WVP;
XMMatrix World;
XMMatrix camView;
XMMatrix camProjection;

XMVector camPosition;
XMVector camTarget;
XMVector camUp;


struct cbPerObject
{
	XMMatrix WVP;
};
VOut VShader(float4 position : POSITION, float4 color : COLOR)
{
	VOut output;

	output.position = position;
	output.color = color;

	return output;
}


float4 PShader(float4 position : SV_POSITION, float4 color : COLOR) : SV_TARGET
{
	return color;
}