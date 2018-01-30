#ifndef __GFX_H
#define __GFX_H

ErrorCode InitD3D(void);
ErrorCode ResetD3D(void);
void FreeD3D(void);
ID3D11Device* GetD3DDevice(void);
ID3D11DeviceContext* GetD3DDeviceContext(void);

BOOL RenderFrame(void);

#endif
