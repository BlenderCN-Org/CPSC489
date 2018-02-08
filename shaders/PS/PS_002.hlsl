// PS_VERTEX_COLOR: 2
// Renders using vertex colors.

struct PixelShaderInput
{
 float4 pos : SV_POSITION;
 float4 col : COLOR;
};

float4 PS(PixelShaderInput input) : SV_TARGET
{
 return input.col;
}
