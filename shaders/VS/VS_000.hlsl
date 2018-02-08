// VS_DEFAULT: 0
// Simply forwards transformed position to pixel shader. Vertices are
// transformed by camera and model matrices.

struct VShaderInput
{
 float4 pos : POSITION;
};

struct PShaderInput
{
 float4 pos : SV_POSITION;
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
 psi.pos = input.pos;
 psi.pos = mul(psi.pos, mview);
 psi.pos = mul(psi.pos, cview);
 return psi;
}