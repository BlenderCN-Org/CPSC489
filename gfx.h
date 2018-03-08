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

// Camera Functions
ErrorCode InitCamera(void);
void FreeCamera(void);
ID3D11Buffer* GetCamera(void);
ErrorCode UpdateCamera(void);

// Identity Matrix Functions
ErrorCode InitIdentityMatrix(void);
void FreeIdentityMatrix(void);
ID3D11Buffer* GetIdentityMatrix(void);

// Rendering Functions
BOOL RenderFrame(real32 dt);

// Vertex Buffer Functions
ErrorCode CreateVertexBuffer(LPVOID data, DWORD n, DWORD stride, ID3D11Buffer** buffer, D3D11_USAGE usage);
ErrorCode CreateVertexBuffer(LPVOID data, DWORD n, DWORD stride, ID3D11Buffer** buffer);
ErrorCode CreateDynamicVertexBuffer(LPVOID data, DWORD n, DWORD stride, ID3D11Buffer** buffer);
ErrorCode CreateImmutableVertexBuffer(LPVOID data, DWORD n, DWORD stride, ID3D11Buffer** buffer);

// Index Buffer Functions
ErrorCode CreateIndexBuffer(LPVOID data, DWORD n, DWORD stride, ID3D11Buffer** buffer, D3D11_USAGE usage);
ErrorCode CreateIndexBuffer(LPVOID data, DWORD n, DWORD stride, ID3D11Buffer** buffer);
ErrorCode CreateDynamicIndexBuffer(LPVOID data, DWORD n, DWORD stride, ID3D11Buffer** buffer);
ErrorCode CreateImmutableIndexBuffer(LPVOID data, DWORD n, DWORD stride, ID3D11Buffer** buffer);

// Constant Buffer Functions
ErrorCode CreateDynamicConstBuffer(ID3D11Buffer** buffer, UINT size);
ErrorCode CreateDynamicConstBuffer(ID3D11Buffer** buffer, UINT size, const void* data);
ErrorCode UpdateDynamicConstBuffer(ID3D11Buffer* buffer, UINT size, const void* data);
ErrorCode CreateImmutableConstBuffer(ID3D11Buffer** buffer, UINT size, const void* data);
ErrorCode CreateDynamicFloat4ConstBuffer(real32* color, ID3D11Buffer** buffer);
ErrorCode UpdateDynamicFloat4ConstBuffer(ID3D11Buffer* buffer, const real32* color);
ErrorCode CreateImmutableFloat4ConstBuffer(real32* color, ID3D11Buffer** buffer);
ErrorCode CreateDynamicMatrixConstBuffer(ID3D11Buffer** buffer);
ErrorCode UpdateDynamicMatrixConstBuffer(ID3D11Buffer* buffer, const DirectX::XMMATRIX& m);
ErrorCode CreateDynamicMatrixConstBuffer(ID3D11Buffer** buffer, UINT n);
ErrorCode UpdateDynamicMatrixConstBuffer(ID3D11Buffer* buffer, UINT n, const DirectX::XMMATRIX* m);
ErrorCode CreateImmutableMatrixConstBuffer(ID3D11Buffer** buffer, const real32* m);

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
