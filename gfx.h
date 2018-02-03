#ifndef __GFX_H
#define __GFX_H

#define IL_P4                   0x00
#define IL_P4_C4                0x01
#define IL_P4_C4_M4             0x02
#define IL_P4_T2                0x03
#define IL_P4_T2_T2             0x04
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

// Direct3D Initialization Functions
ErrorCode InitD3D(void);
ErrorCode ResetD3D(UINT dx, UINT dy);
void FreeD3D(void);
ID3D11Device* GetD3DDevice(void);
ID3D11DeviceContext* GetD3DDeviceContext(void);

// Render Target Functions
ErrorCode InitRenderTarget(UINT dx = 0, UINT dy = 0);
void FreeRenderTarget(void);

// Vertex Shader Functions
ErrorCode InitVertexShaders(void);
void FreeVertexShaders(void);

// Input Layout Functions
ErrorCode InitInputLayouts(void);
void FreeInputLayouts(void);

// Rendering Functions
BOOL RenderFrame(void);

#endif
