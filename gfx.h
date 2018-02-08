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
ErrorCode InitPerCameraBuffer(void);
void FreePerCameraBuffer(void);
ID3D11Buffer* GetPerCameraBuffer(void);

// Rendering Functions
BOOL RenderFrame(void);

// Buffer Functions
ErrorCode CreateVertexBuffer(LPVOID data, DWORD n, DWORD stride, ID3D11Buffer** buffer, D3D11_USAGE usage);
ErrorCode CreateVertexBuffer(LPVOID data, DWORD n, DWORD stride, ID3D11Buffer** buffer);
ErrorCode CreateDynamicVertexBuffer(LPVOID data, DWORD n, DWORD stride, ID3D11Buffer** buffer);
ErrorCode CreateImmutableVertexBuffer(LPVOID data, DWORD n, DWORD stride, ID3D11Buffer** buffer);
ErrorCode CreateIndexBuffer(LPVOID data, DWORD n, DWORD stride, ID3D11Buffer** buffer, D3D11_USAGE usage);
ErrorCode CreateIndexBuffer(LPVOID data, DWORD n, DWORD stride, ID3D11Buffer** buffer);
ErrorCode CreateDynamicIndexBuffer(LPVOID data, DWORD n, DWORD stride, ID3D11Buffer** buffer);
ErrorCode CreateImmutableIndexBuffer(LPVOID data, DWORD n, DWORD stride, ID3D11Buffer** buffer);

// Shader Functions
void SetVertexShaderPerCameraBuffer(ID3D11Buffer* buffer);
void SetVertexShaderPerModelBuffer(ID3D11Buffer* buffer);
void SetVertexShaderPerFrameBuffer(ID3D11Buffer* buffer);
void SetPixelShaderPerCameraBuffer(ID3D11Buffer* buffer);
void SetPixelShaderPerModelBuffer(ID3D11Buffer* buffer);
void SetPixelShaderPerFrameBuffer(ID3D11Buffer* buffer);
void SetShaderResources(UINT n, ID3D11ShaderResourceView** views);

// Input Assembly Functions
void SetVertexBuffer(ID3D11Buffer* buffer, UINT stride, UINT offset);
void SetVertexBufferArray(ID3D11Buffer** buffer, DWORD n, const UINT* stride, const UINT* offset);
void SetIndexBuffer(ID3D11Buffer* buffer, UINT offset, DXGI_FORMAT format);
void SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY topology);
void DrawPointList(UINT vertices);
void DrawLineList(UINT vertices);
void DrawLineList(UINT vertices, UINT start);
void DrawPointList(UINT vertices, UINT start);
void DrawIndexedLineList(UINT indices);
void DrawIndexedTriangleList(UINT indices);

#endif
