// PS_DEFAULT: 0
// Takes position and renders using a default color.

struct PixelShaderInput
{
 float4 pos : SV_POSITION;
};

float4 PS(PixelShaderInput input) : SV_TARGET
{
 return float4(1.0f, 1.0f, 0.0f, 1.0f);
}
