// To compile VS: fxc filename.hlsl /Tvs_5_0 /EVS /Fo filename.cso
// To compile PS: fxc filename.hlsl /Tps_5_0 /EPS /Fo filename.cso

struct VShaderInput
{
 float4 pos : POSITION;
 float4 col : COLOR;
 column_major float4x4 transform : TRANSFORM;
 float4 size : SCALE;
};

struct PShaderInput
{
 float4 pos : SV_POSITION;
 float4 col : COLOR;
};

cbuffer percam : register(b0)
{
 matrix cview;
};

PShaderInput VS(VShaderInput input)
{
 PShaderInput psi;
 psi.pos = input.pos;
 psi.pos.x = psi.pos.x*input.size.x;
 psi.pos.y = psi.pos.y*input.size.x;
 psi.pos.z = psi.pos.z*input.size.x;
 psi.pos = mul(psi.pos, input.transform);
 psi.pos = mul(psi.pos, cview);
 psi.col = input.col;
 return psi;
}