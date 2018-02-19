// To compile VS: fxc filename.hlsl /Tvs_5_0 /EVS /Fo filename.cso
// To compile PS: fxc filename.hlsl /Tps_5_0 /EPS /Fo filename.cso

struct VShaderInput
{
 float4 position : POSITION;
 float4 normal   : NORMAL;
 float2 tex1     : TEXCOORD0;
 float2 tex2     : TEXCOORD1;
 uint4  bi       : BLENDINDICES;
 float4 bw       : BLENDWEIGHTS;
 float4 color1   : COLOR0;
 float4 color2   : COLOR1;
};

struct PShaderInput
{
 float4 position : SV_POSITION;
 float4 normal   : NORMAL;
 float2 tex1     : TEXCOORD0;
 float2 tex2     : TEXCOORD1;
 float4 color1   : COLOR0;
 float4 color2   : COLOR1;
};

cbuffer percam : register(b0)
{
 matrix cview;
};

cbuffer permdl : register(b1)
{
 matrix mview;
};

cbuffer perfrm : register(b2)
{
 matrix mskin[96];
};
 
PShaderInput VS(VShaderInput input)
{
 PShaderInput psi;
 psi.position = input.position;
 psi.position = mul(psi.position, mview);
 psi.position = mul(psi.position, cview);
 psi.normal = input.normal;
 psi.normal = mul(psi.normal, mview);
 psi.normal = mul(psi.normal, cview);
 psi.tex1 = input.tex1;
 psi.tex2 = input.tex2;
 psi.color1 = input.color1;
 psi.color2 = input.color2;
 return psi;
}