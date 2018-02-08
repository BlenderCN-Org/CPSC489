// VS_VERTEX_COLOR: 1
// Simply forwards transformed position and vertex color data to the pixel
// shader. Vertices are transformed by camera and model matrices.

struct VShaderInput
{
 float4 pos : POSITION;
 float4 col : COLOR;
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
 psi.col = input.col;
 return psi;
}