#ifndef __CPSC489_LAYOUTS_H
#define __CPSC489_LAYOUTS_H

// Input Layout Signatures
#define IL_P4                   0x00 // <float4>
#define IL_P4_C4                0x01 // <float4> <float4> 
#define IL_P4_C4_M4_S1          0x02 // <float4> <float4> <matrix> <float>
#define IL_P4_T2                0x03 // <float4> <float2>
#define IL_P4_T2_T2             0x04 // <float4> <float2> <float2>
#define IL_P4_N4_T2             0x05
#define IL_P4_N4_T2_T2          0x06
#define IL_P4_N4_B4_T2          0x07
#define IL_P4_N4_B4_T2_T2       0x08
#define IL_P4_T2_I4_W4          0x09
#define IL_P4_T2_T2_I4_W4       0x0A
#define IL_P4_N4_T2_I4_W4       0x0B
#define IL_P4_N4_T2_T2_I4_W4    0x0C
#define IL_P4_N4_B4_T2_I4_W4    0x0D
#define IL_P4_N4_B4_T2_T2_I4_W4 0x0E

// Input Layout Functions
ErrorCode InitInputLayouts(void);
void FreeInputLayouts(void);
ID3D11InputLayout* GetInputLayout(size_t index);
ErrorCode SetInputLayout(size_t index);

#endif
