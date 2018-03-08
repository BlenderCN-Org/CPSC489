// To compile VS: fxc filename.hlsl /Tvs_5_0 /EVS /Fo filename.cso
// To compile PS: fxc filename.hlsl /Tps_5_0 /EPS /Fo filename.cso

struct VShaderInput
{
 float4 position : POSITION;
 float4 center   : CENTER;
 float4 halfdims : HALFDIMS;
};

struct PShaderInput
{
 float4 position : SV_POSITION;
 float4 color    : COLOR;
};

cbuffer percam : register(b0)
{
 matrix cview;
};

cbuffer permdl : register(b1)
{
 matrix mview;
 float4 color;
};

PShaderInput VS(VShaderInput input)
{
 PShaderInput psi;
 psi.position = input.position * input.halfdims + input.center;
 psi.position = mul(psi.position, mview);
 psi.position = mul(psi.position, cview);
 psi.color = color;
 return psi;
}