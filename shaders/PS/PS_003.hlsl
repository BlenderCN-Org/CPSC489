Texture2D t2d;
SamplerState ss;

struct PixelShaderInput
{
 float4 position : SV_POSITION;
 float4 normal   : NORMAL;
 float2 tex1     : TEXCOORD0;
 float2 tex2     : TEXCOORD1;
 float4 color1   : COLOR0;
 float4 color2   : COLOR1;
};

float4 PS(PixelShaderInput input) : SV_TARGET
{
 float4 color = t2d.Sample(ss, input.tex1);
 return color;
}
