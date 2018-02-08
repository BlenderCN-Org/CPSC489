// PS_CONST_COLOR: 1
// Renders using a constant color in slot 1.

struct PixelShaderInput
{
 float4 pos : SV_POSITION;
};

cbuffer permdl : register(b1)
{
 float4 col : COLOR;
};

float4 PS(PixelShaderInput input) : SV_TARGET
{
 return col;
}