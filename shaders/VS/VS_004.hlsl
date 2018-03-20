// To compile VS: fxc filename.hlsl /Tvs_5_0 /EVS /Fo filename.cso
// To compile PS: fxc filename.hlsl /Tps_5_0 /EPS /Fo filename.cso

struct VShaderInput
{
 float4 position : POSITION;
 float4 minedge  : MINEDGE;
 float4 maxedge  : MAXEDGE;
};

struct PShaderInput
{
 float4 position : SV_POSITION;
};

cbuffer percam : register(b0)
{
 matrix cview;
};

cbuffer permdl : register(b1)
{
 matrix mview;
};

PShaderInput VS(VShaderInput input)
{
 PShaderInput psi;
 float3 scalef = (input.maxedge.xyz - input.minedge.xyz)/2.0;
 float3 center = (input.minedge.xyz + input.maxedge.xyz)/2.0;
 psi.position = input.position * float4(scalef, 1.0);
 psi.position = psi.position + float4(center, 0.0);
 psi.position = mul(psi.position, mview);
 psi.position = mul(psi.position, cview);
 return psi;
}