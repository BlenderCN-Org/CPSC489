#ifndef __CPSC489_GFX_H
#define __CPSC489_GFX_H

// Direct3D Initialization Functions
ErrorCode InitD3D(void);
ErrorCode ResetD3D(UINT dx, UINT dy);
void FreeD3D(void);
ID3D11Device* GetD3DDevice(void);
ID3D11DeviceContext* GetD3DDeviceContext(void);

// Render Target Functions
ErrorCode InitRenderTarget(UINT dx = 0, UINT dy = 0);
void FreeRenderTarget(void);

// Matrix Functions
ErrorCode InitViewProjectionMatrix(void);
void FreeViewProjectionMatrix(void);
ID3D11Buffer* GetViewProjectionMatrix(void);

// Rendering Functions
BOOL RenderFrame(void);

#endif
