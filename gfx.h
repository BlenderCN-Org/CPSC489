#ifndef __GFX_H
#define __GFX_H

// Direct3D Initialization Functions
ErrorCode InitD3D(void);
ErrorCode ResetD3D(UINT dx, UINT dy);
void FreeD3D(void);
ID3D11Device* GetD3DDevice(void);
ID3D11DeviceContext* GetD3DDeviceContext(void);

// Render Target Functions
ErrorCode InitRenderTarget(UINT dx = 0, UINT dy = 0);
void FreeRenderTarget(void);

// Input Layout Functions
ErrorCode InitInputLayouts(void);
void FreeInputLayouts(void);

// Rendering Functions
BOOL RenderFrame(void);

#endif
