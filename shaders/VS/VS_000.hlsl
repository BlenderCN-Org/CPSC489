// To compile VS: fxc filename.hlsl /Tvs_5_0 /EVS /Fo filename.cso
// To compile PS: fxc filename.hlsl /Tps_5_0 /EPS /Fo filename.cso

struct VShaderInput
{
 float4 pos : POSITION;
};

struct PShaderInput
{
 float4 pos : SV_POSITION;
};

cbuffer perframe : register(b0)
{
 matrix pview;
 matrix wview;
};

cbuffer permodel : register(b1)
{
 matrix mview;
};

PShaderInput VS(VShaderInput input)
{
 PShaderInput psi;
 psi.pos = input.pos;
 psi.pos = mul(psi.pos, mview);
 psi.pos = mul(psi.pos, wview);
 psi.pos = mul(psi.pos, pview);
 return psi;
}