// To compile VS: fxc filename.hlsl /Tvs_5_0 /EVS /Fo filename.cso
// To compile PS: fxc filename.hlsl /Tps_5_0 /EPS /Fo filename.cso

struct PixelShaderInput
{
 float4 pos : SV_POSITION;
};

float4 PS(PixelShaderInput input) : SV_TARGET
{
 return float4(1.0f, 1.0f, 0.0f, 1.0f);
}
