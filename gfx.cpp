#include "stdafx.h"
#include "errors.h"
#include "app.h"
#include "win.h"
#include "camera.h"
#include "layouts.h"
#include "shaders.h"
#include "axes.h"
#include "grid.h"
#include "orbit.h"
#include "gfx.h"
#include "testing/tests.h"

// Direct3D Variables
static ID3D11Device* lpDevice;
static ID3D11DeviceContext* lpDeviceContext;
static IDXGISwapChain* lpSwapChain;
static UINT buffer_dx = 0;
static UINT buffer_dy = 0;
static ID3D11RenderTargetView* lpRenderTargetView;
static ID3D11Texture2D* lpDepthTexture;
static ID3D11DepthStencilView* lpDepthStencil;

// Direct3D Buffer Variables
static ID3D11Buffer* lpMatrix = NULL;
static ID3D11Buffer* lpIdentityMatrix = NULL;

#pragma region DIRECT3D_INIT_FUNCTIONS

ErrorCode InitD3D(void)
{
 // already exists
 if(lpDevice) return EC_SUCCESS;

 //
 // PHASE 1: INITIALIZE RENDER TARGET
 //

 // initialize swap chain descriptor
 DXGI_SWAP_CHAIN_DESC scd;
 ZeroMemory(&scd, sizeof(scd));
 scd.BufferCount = 1;
 scd.BufferDesc.Width = 0;
 scd.BufferDesc.Height = 0;
 scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
 scd.BufferDesc.RefreshRate.Numerator = 60;
 scd.BufferDesc.RefreshRate.Denominator = 1;
 scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
 scd.OutputWindow = GetMainWindow();
 scd.SampleDesc.Count = 1;
 scd.SampleDesc.Quality = 0;
 scd.Windowed = TRUE;

 // creation flags
 DWORD cflags = 0;
 #if defined(_DEBUG)
 cflags |= D3D11_CREATE_DEVICE_DEBUG;
 #endif

 // create device and swap chain
 D3D_FEATURE_LEVEL featureLevelIn = D3D_FEATURE_LEVEL_11_0;
 D3D_FEATURE_LEVEL featureLevelOut;
 HRESULT result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, cflags, &featureLevelIn, 1, D3D11_SDK_VERSION, &scd, &lpSwapChain, &lpDevice, &featureLevelOut, &lpDeviceContext);
 if(FAILED(result)) return EC_D3D_CREATE_DEVICE;

 // create render target (color + depth + stencil buffer)
 ErrorCode code = InitRenderTarget();
 if(Fail(code)) return code;

 //
 // PHASE 2: INITIALIZE SHADERS
 //

 // create vertex shaders
 code = InitVertexShaders();
 if(Fail(code)) return code;

 // create pixel shaders
 code = InitPixelShaders();
 if(Fail(code)) return code;

 // create input layouts (after vertex shader creation)
 code = InitInputLayouts();
 if(Fail(code)) return code;

 //
 // PHASE 3: INITIALIZE CONSTANT BUFFERS
 //

 // create identity matrix
 code = InitIdentityMatrix();
 if(Fail(code)) return code;

 // create view projection matrix
 code = InitCamera();
 if(Fail(code)) return code;

 //
 // PHASE 4: INITIALIZE DEFAULT MODELS
 //

 // create grid
 code = InitGrid();
 if(Fail(code)) return code;

 // create axes
 code = InitAxesModel();
 if(Fail(code)) return code;

 // create orbit box
 code = InitOrbitBox();
 if(Fail(code)) return code;

 //
 // PHASE 5: PREPARE FOR RENDERING
 //

 // set viewport and update camera before rendering
 auto orbitcam = GetOrbitCamera();
 orbitcam->SetViewport(0, 0, (int)buffer_dx, (int)buffer_dy);
 UpdateCamera();

 // success
 return EC_SUCCESS;
}

/**
 * \fn      ResetD3D
 * \brief   Resets the default Direct3D objects when window is resized.
 * \details When a window is resized, the render target, along with its associated buffers,
 *          must be recreated. The back buffer(s), which are part of the swap chain, must
 *          be resized as well.
 * \return  Returns an error code.
 */
ErrorCode ResetD3D(UINT dx, UINT dy)
{
 // resize render target (buffers)
 ErrorCode code = InitRenderTarget(dx, dy);
 if(Fail(code)) return code;

 // set viewport
 auto orbitcam = GetOrbitCamera();
 orbitcam->SetViewport(0, 0, (int)buffer_dx, (int)buffer_dy);
 UpdateCamera();

 return EC_SUCCESS;
}

void FreeD3D(void)
{
 // stop any testing
 if(IsTestActive()) EndTest();

 // release default models
 FreeOrbitBox();
 FreeAxesModel();
 FreeGrid();

 // release camera
 FreeCamera();

 // release identity matrix
 FreeIdentityMatrix();

 // release shaders
 FreeInputLayouts();
 FreePixelShaders();
 FreeVertexShaders();

 // release framebuffer objects
 FreeRenderTarget();

 // release device objects (from D3D11CreateDeviceAndSwapChain)
 if(lpSwapChain) {
    lpSwapChain->Release();
    lpSwapChain = NULL;
   }
 if(lpDeviceContext) {
    lpDeviceContext->Release();
    lpDeviceContext = NULL;
   }
 if(lpDevice) {
    lpDevice->Release();
    lpDevice = NULL;
   }
}

ID3D11Device* GetD3DDevice(void)
{
 return lpDevice;
}

ID3D11DeviceContext* GetD3DDeviceContext(void)
{
 return lpDeviceContext;
}

#pragma endregion DIRECT3D_INIT_FUNCTIONS

#pragma region RENDER_TARGET_FUNCTIONS

ErrorCode InitRenderTarget(UINT dx, UINT dy)
{
 // stop rendering
 if(!lpDeviceContext) return EC_D3D_DEVICE_CONTEXT;
 lpDeviceContext->OMSetRenderTargets(0, NULL, NULL);

 // free previous
 FreeRenderTarget();

 // resize display buffers
 HRESULT result;
 if(dx && dy) {
    result = lpSwapChain->ResizeBuffers(0, dx, dy, DXGI_FORMAT_UNKNOWN, 0);
    if(FAILED(result)) return EC_D3D_RESIZE_BUFFERS;
   }

 // get interface to back buffer
 ID3D11Texture2D* lpBackBuffer = NULL;
 result = lpSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&lpBackBuffer);
 if(FAILED(result)) return EC_D3D_GET_BACKBUFFER;

 // get backbuffer descriptor and dimensions
 D3D11_TEXTURE2D_DESC bbd = {0};
 lpBackBuffer->GetDesc(&bbd);
 buffer_dx = bbd.Width;
 buffer_dy = bbd.Height;

 // create render target view
 result = lpDevice->CreateRenderTargetView(lpBackBuffer, NULL, &lpRenderTargetView);
 if(FAILED(result)) return EC_D3D_CREATE_RENDER_TARGET_VIEW;

 // back buffer no longer needed
 lpBackBuffer->Release();
 lpBackBuffer = NULL;

 // fill depth buffer parameters
 D3D11_TEXTURE2D_DESC dsd;
 ZeroMemory(&dsd, sizeof(dsd));
 dsd.Width = bbd.Width;
 dsd.Height = bbd.Height;
 dsd.MipLevels = 1;
 dsd.ArraySize = 1;
 dsd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
 dsd.SampleDesc.Count = 1;
 dsd.SampleDesc.Quality = 0;
 dsd.Usage = D3D11_USAGE_DEFAULT;
 dsd.BindFlags = D3D11_BIND_DEPTH_STENCIL;

 // create depth buffer
 if(FAILED(lpDevice->CreateTexture2D(&dsd, nullptr, &lpDepthTexture)))
    return EC_D3D_CREATE_TEXTURE2D;

// create depth stencil view
 D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
 ZeroMemory(&dsvd, sizeof(dsvd));
 dsvd.Format = dsd.Format;
 dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
 if(FAILED(lpDevice->CreateDepthStencilView(lpDepthTexture, &dsvd, &lpDepthStencil)))
    return EC_D3D_CREATE_DEPTH_STENCIL_VIEW;

 // set render target
 lpDeviceContext->OMSetRenderTargets(1, &lpRenderTargetView, lpDepthStencil);

 // set viewport
 D3D11_VIEWPORT vp = { 0.0f, 0.0f, (FLOAT)buffer_dx, (FLOAT)buffer_dy, 0.0f, 1.0f };
 lpDeviceContext->RSSetViewports(1, &vp);

 return EC_SUCCESS;
}

void FreeRenderTarget(void)
{
 // release depth texture
 if(lpDepthTexture) {
    lpDepthTexture->Release();
    lpDepthTexture = NULL;
   }

 // release depth stencil view
 if(lpDepthStencil) {
    lpDepthStencil->Release();
    lpDepthStencil = NULL;
   }

 // release render target view
 if(lpRenderTargetView) {
    lpRenderTargetView->Release();
    lpRenderTargetView = NULL;
   }
}

#pragma endregion RENDER_TARGET_FUNCTIONS

#pragma region CAMERA_FUNCTIONS

ErrorCode InitCamera(void)
{
 // already exists
 if(lpMatrix) return EC_SUCCESS;

 // create matrix buffer
 ErrorCode code = CreateDynamicMatrixConstBuffer(&lpMatrix);
 if(Fail(code)) return code;

 // set camera parameters
 auto orbitcam = GetOrbitCamera();
 orbitcam->SetViewport(0, 0, (int)buffer_dx, (int)buffer_dy);
 orbitcam->Reset();

 // update matrix buffer
 return EC_SUCCESS;
}

void FreeCamera(void)
{
 if(lpMatrix) lpMatrix->Release();
 lpMatrix = nullptr;
}

ID3D11Buffer* GetCamera(void)
{
 return lpMatrix;
}

ErrorCode UpdateCamera(void)
{
 // get clipping plane coordinates
 real32 coords[4];
 auto orbitcam = GetOrbitCamera();
 orbitcam->GetClippingPlaneCoords(coords);

 // calculate perspective matrix
 using namespace DirectX;
 float dx = 2*coords[1];
 float dy = 2*coords[3];
 float zn = orbitcam->GetNearPlane();
 float zf = orbitcam->GetFarPlane();
 XMMATRIX P;
 P = XMMatrixPerspectiveRH(dx, dy, zn, zf);

 // calculate view matrix
 const real32* cam_E = orbitcam->GetCameraOrigin();
 const real32* cam_X = orbitcam->GetCameraXAxis();
 const real32* cam_Y = orbitcam->GetCameraYAxis();
 const real32* cam_Z = orbitcam->GetCameraZAxis();
 XMVECTOR E = XMVectorSet(cam_E[0], cam_E[1], cam_E[2], 0.0f);
 XMVECTOR X = XMVectorSet(cam_X[0], cam_X[1], cam_X[2], 0.0f);
 XMVECTOR Y = XMVectorSet(cam_Y[0], cam_Y[1], cam_Y[2], 0.0f);
 XMVECTOR Z = XMVectorSet(cam_Z[0], cam_Z[1], cam_Z[2], 0.0f);
 XMMATRIX V = XMMatrixLookToRH(E, X, Z);

 // calculate perspective view matrix
 XMMATRIX R = XMMatrixMultiply(V, P);
 R = XMMatrixTranspose(R);

 // update Direct3D resource
 ErrorCode code = UpdateDynamicMatrixConstBuffer(lpMatrix, R);
 if(Fail(code)) return code;
 SetVertexShaderPerCameraBuffer(lpMatrix);

 // update orbit box
 UpdateOrbitBox();
 return EC_SUCCESS;
}

#pragma endregion CAMERA_FUNCTIONS

#pragma region IDENTITY_MATRIX_FUNCTIONS

ErrorCode InitIdentityMatrix(void)
{
 // create matrix buffer
 if(lpIdentityMatrix) return EC_SUCCESS; // already exists
 float m[16] = {
  1.0f, 0.0f, 0.0f, 0.0f,
  0.0f, 1.0f, 0.0f, 0.0f,
  0.0f, 0.0f, 1.0f, 0.0f,
  0.0f, 0.0f, 0.0f, 1.0f,
 };
 return CreateImmutableMatrixConstBuffer(&lpIdentityMatrix, &m[0]);
}

void FreeIdentityMatrix(void)
{
 if(lpIdentityMatrix) {
    lpIdentityMatrix->Release();
    lpIdentityMatrix = nullptr;
   }
}

ID3D11Buffer* GetIdentityMatrix(void)
{
 return lpIdentityMatrix;
}

#pragma endregion IDENTITY_MATRIX_FUNCTIONS

#pragma region RENDERING_FUNCTIONS

BOOL RenderFrame(void)
{
 // validate
 if(!lpDeviceContext) return FALSE;
 if(!lpSwapChain) return FALSE;

 // clear buffers
 FLOAT color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
 lpDeviceContext->ClearRenderTargetView(lpRenderTargetView, color);
 lpDeviceContext->ClearDepthStencilView(lpDepthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

 // render default models
 RenderGrid();
 RenderOrbitBox();

 // render test
 if(GetActiveTest() != -1)
    RenderTest();

 // present
 lpSwapChain->Present(0, 0);

 return TRUE;
}

#pragma endregion RENDERING_FUNCTIONS

#pragma region DIRECT3D_VERTEX_BUFFER_FUNCTIONS

ErrorCode CreateVertexBuffer(LPVOID data, DWORD n, DWORD stride, ID3D11Buffer** buffer, D3D11_USAGE usage)
{
 // must have device
 ID3D11Device* device = GetD3DDevice();
 if(!device) return EC_D3D_DEVICE;

 // vertex buffer descriptor
 D3D11_BUFFER_DESC bd;
 ZeroMemory(&bd, sizeof(bd));
 bd.Usage = usage;
 bd.ByteWidth = n * stride;
 bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
 bd.CPUAccessFlags = 0;

 // create vertex buffer
 D3D11_SUBRESOURCE_DATA srd;
 ZeroMemory(&srd, sizeof(srd));
 srd.pSysMem = data;
 HRESULT result = lpDevice->CreateBuffer(&bd, &srd, buffer);
 return (SUCCEEDED(result) ?  EC_SUCCESS : EC_D3D_CREATE_BUFFER);
}

ErrorCode CreateVertexBuffer(LPVOID data, DWORD n, DWORD stride, ID3D11Buffer** buffer)
{
 return CreateVertexBuffer(data, n, stride, buffer, D3D11_USAGE_DEFAULT);
}

ErrorCode CreateDynamicVertexBuffer(LPVOID data, DWORD n, DWORD stride, ID3D11Buffer** buffer)
{
 // must have device
 ID3D11Device* device = GetD3DDevice();
 if(!device) return EC_D3D_DEVICE;

 // vertex buffer descriptor
 D3D11_BUFFER_DESC bd;
 ZeroMemory(&bd, sizeof(bd));
 bd.Usage = D3D11_USAGE_DYNAMIC;
 bd.ByteWidth = n * stride;
 bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
 bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

 // create vertex buffer
 HRESULT result = lpDevice->CreateBuffer(&bd, NULL, buffer);
 if(FAILED(result)) return EC_D3D_CREATE_BUFFER;

 // D3D11_USAGE_DYNAMIC REQUIRES initialization via ID3D11DeviceContext::Map
 if(data) {
    D3D11_MAPPED_SUBRESOURCE msr;
    ZeroMemory(&msr, sizeof(msr));
    if(!FAILED(lpDeviceContext->Map(*buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr))) {
       memmove(msr.pData, data, bd.ByteWidth);
       lpDeviceContext->Unmap(*buffer, 0);
      }
    else {
       // failed!
       (*buffer)->Release();
       (*buffer) = NULL;
       return EC_D3D_MAP_RESOURCE;
      }
   }

 // success
 return EC_SUCCESS;
}

ErrorCode CreateImmutableVertexBuffer(LPVOID data, DWORD n, DWORD stride, ID3D11Buffer** buffer)
{
 return CreateVertexBuffer(data, n, stride, buffer, D3D11_USAGE_IMMUTABLE);
}

#pragma endregion DIRECT3D_VERTEX_BUFFER_FUNCTIONS

#pragma region DIRECT3D_INDEX_BUFFER_FUNCTIONS

ErrorCode CreateIndexBuffer(LPVOID data, DWORD n, DWORD stride, ID3D11Buffer** buffer, D3D11_USAGE usage)
{
 // must have device
 ID3D11Device* device = GetD3DDevice();
 if(!device) return EC_D3D_DEVICE;

 // index buffer descriptor
 D3D11_BUFFER_DESC bd;
 ZeroMemory(&bd, sizeof(bd));
 bd.Usage = usage;
 bd.ByteWidth = n * stride;
 bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
 bd.CPUAccessFlags = 0;

 // create index buffer
 D3D11_SUBRESOURCE_DATA srd;
 ZeroMemory(&srd, sizeof(srd));
 srd.pSysMem = data;
 HRESULT result = device->CreateBuffer(&bd, &srd, buffer);
 return (SUCCEEDED(result) ? EC_SUCCESS : EC_D3D_CREATE_BUFFER);
}

ErrorCode CreateIndexBuffer(LPVOID data, DWORD n, DWORD stride, ID3D11Buffer** buffer)
{
 return CreateIndexBuffer(data, n, stride, buffer, D3D11_USAGE_DEFAULT);
}

ErrorCode CreateDynamicIndexBuffer(LPVOID data, DWORD n, DWORD stride, ID3D11Buffer** buffer)
{
 // must have device
 ID3D11Device* device = GetD3DDevice();
 if(!device) return EC_D3D_DEVICE;

 // index buffer descriptor
 D3D11_BUFFER_DESC bd;
 ZeroMemory(&bd, sizeof(bd));
 bd.Usage = D3D11_USAGE_DYNAMIC;
 bd.ByteWidth = n * stride;
 bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
 bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

 // create index buffer
 D3D11_SUBRESOURCE_DATA srd;
 ZeroMemory(&srd, sizeof(srd));
 srd.pSysMem = data;
 HRESULT result = device->CreateBuffer(&bd, NULL, buffer);
 if(FAILED(result)) return EC_D3D_CREATE_BUFFER;

 // map index buffer data
 // D3D11_USAGE_DYNAMIC REQUIRES initialization via ID3D11DeviceContext::Map
 if(data) {
    D3D11_MAPPED_SUBRESOURCE msr;
    ZeroMemory(&msr, sizeof(msr));
    if(!FAILED(lpDeviceContext->Map(*buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr))) {
       memcpy(msr.pData, data, bd.ByteWidth);
       lpDeviceContext->Unmap(*buffer, 0);
      }
    else {
       // failed!
       (*buffer)->Release();
       (*buffer) = NULL;
       return EC_D3D_MAP_RESOURCE;
      }
   }

 // success
 return EC_SUCCESS;
}

ErrorCode CreateImmutableIndexBuffer(LPVOID data, DWORD n, DWORD stride, ID3D11Buffer** buffer)
{
 return CreateIndexBuffer(data, n, stride, buffer, D3D11_USAGE_IMMUTABLE);
}

#pragma endregion DIRECT3D_INDEX_BUFFER_FUNCTIONS

#pragma region DIRECT3D_CONSTANT_BUFFER_FUNCTIONS

ErrorCode CreateDynamicFloat4ConstBuffer(real32* color, ID3D11Buffer** buffer)
{
 // create buffer
 D3D11_BUFFER_DESC desc;
 ZeroMemory(&desc, sizeof(desc));
 desc.Usage = D3D11_USAGE_DYNAMIC;
 desc.ByteWidth = 16;
 desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
 desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
 HRESULT result = lpDevice->CreateBuffer(&desc, NULL, buffer);
 if(FAILED(result)) return EC_D3D_CREATE_BUFFER;

 // map data
 D3D11_MAPPED_SUBRESOURCE msr;
 ZeroMemory(&msr, sizeof(msr));
 if(!FAILED(lpDeviceContext->Map(*buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr))) {
    real32* data = reinterpret_cast<real32*>(msr.pData);
    data[0] = color[0];
    data[1] = color[1];
    data[2] = color[2];
    data[3] = color[3];
    lpDeviceContext->Unmap(*buffer, 0);
   }
 else {
    // failed!
    (*buffer)->Release();
    (*buffer) = NULL;
    return EC_D3D_MAP_RESOURCE;
   }

 return EC_SUCCESS;
}

ErrorCode UpdateDynamicFloat4ConstBuffer(ID3D11Buffer* buffer, const real32* color)
{
 D3D11_MAPPED_SUBRESOURCE msr;
 ZeroMemory(&msr, sizeof(msr));
 if(FAILED(lpDeviceContext->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr))) return EC_D3D_MAP_RESOURCE;
 real32* data = reinterpret_cast<real32*>(msr.pData);
 data[0] = color[0];
 data[1] = color[1];
 data[2] = color[2];
 data[3] = color[3];
 lpDeviceContext->Unmap(buffer, 0);
 return EC_SUCCESS;
} 

ErrorCode CreateDynamicMatrixConstBuffer(ID3D11Buffer** buffer)
{
 // initialize descriptor
 D3D11_BUFFER_DESC desc;
 ZeroMemory(&desc, sizeof(desc));
 desc.Usage = D3D11_USAGE_DYNAMIC;
 desc.ByteWidth = sizeof(DirectX::XMMATRIX);
 desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
 desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

 // create buffer
 HRESULT result = lpDevice->CreateBuffer(&desc, NULL, buffer);
 if(FAILED(result)) return EC_D3D_CREATE_BUFFER;
 return EC_SUCCESS;
}

ErrorCode UpdateDynamicMatrixConstBuffer(ID3D11Buffer* buffer, const DirectX::XMMATRIX& m)
{
 D3D11_MAPPED_SUBRESOURCE msr;
 ZeroMemory(&msr, sizeof(msr));
 if(FAILED(lpDeviceContext->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr))) return EC_D3D_MAP_RESOURCE;
 DirectX::XMMATRIX* data = reinterpret_cast<DirectX::XMMATRIX*>(msr.pData);
 data[0] = m;
 lpDeviceContext->Unmap(buffer, 0);
 return EC_SUCCESS;
}

ErrorCode CreateDynamicMatrixConstBuffer(ID3D11Buffer** buffer, UINT n)
{
 // must have n
 if(!n) return EC_D3D_CREATE_BUFFER;

 // initialize descriptor
 D3D11_BUFFER_DESC desc;
 ZeroMemory(&desc, sizeof(desc));
 desc.Usage = D3D11_USAGE_DYNAMIC;
 desc.ByteWidth = n*sizeof(DirectX::XMMATRIX);
 desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
 desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

 // create buffer
 HRESULT result = lpDevice->CreateBuffer(&desc, NULL, buffer);
 if(FAILED(result)) return EC_D3D_CREATE_BUFFER;
 return EC_SUCCESS;
}

ErrorCode UpdateDynamicMatrixConstBuffer(ID3D11Buffer* buffer, UINT n, const DirectX::XMMATRIX* m)
{
 // must have n
 if(!n) return EC_D3D_CREATE_BUFFER;
 D3D11_MAPPED_SUBRESOURCE msr;
 ZeroMemory(&msr, sizeof(msr));
 if(FAILED(lpDeviceContext->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr))) return EC_D3D_MAP_RESOURCE;
 DirectX::XMMATRIX* data = reinterpret_cast<DirectX::XMMATRIX*>(msr.pData);
 memmove(data, m, n*sizeof(DirectX::XMMATRIX));
 lpDeviceContext->Unmap(buffer, 0);
 return EC_SUCCESS;
}

ErrorCode CreateImmutableMatrixConstBuffer(ID3D11Buffer** buffer, const real32* m)
{
 // buffer description
 D3D11_BUFFER_DESC bd;
 ZeroMemory(&bd, sizeof(bd));
 bd.Usage = D3D11_USAGE_IMMUTABLE;
 bd.ByteWidth = sizeof(DirectX::XMMATRIX);
 bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
 bd.CPUAccessFlags = 0;

 // initialize data
 D3D11_SUBRESOURCE_DATA srd;
 ZeroMemory(&srd, sizeof(srd));
 auto matrix = DirectX::XMMatrixIdentity();
 srd.pSysMem = m;

 // create buffer
 if(FAILED(lpDevice->CreateBuffer(&bd, &srd, buffer))) return EC_D3D_CREATE_BUFFER;
 return EC_SUCCESS;
}

#pragma endregion DIRECT3D_BUFFER_FUNCTIONS

#pragma region DIRECT3D_SHADER_FUNCTIONS

void SetVertexShaderPerCameraBuffer(ID3D11Buffer* buffer)
{
 if(lpDeviceContext && buffer) lpDeviceContext->VSSetConstantBuffers(0, 1, &buffer);
}

void SetVertexShaderPerModelBuffer(ID3D11Buffer* buffer)
{
 if(lpDeviceContext && buffer) lpDeviceContext->VSSetConstantBuffers(1, 1, &buffer);
}

void SetVertexShaderPerFrameBuffer(ID3D11Buffer* buffer)
{
 if(lpDeviceContext && buffer) lpDeviceContext->VSSetConstantBuffers(2, 1, &buffer);
}

void SetPixelShaderPerCameraBuffer(ID3D11Buffer* buffer)
{
 if(lpDeviceContext) lpDeviceContext->PSSetConstantBuffers(0, 1, &buffer);
}

void SetPixelShaderPerModelBuffer(ID3D11Buffer* buffer)
{
 if(lpDeviceContext) lpDeviceContext->PSSetConstantBuffers(1, 1, &buffer);
}

void SetPixelShaderPerFrameBuffer(ID3D11Buffer* buffer)
{
 if(lpDeviceContext) lpDeviceContext->PSSetConstantBuffers(2, 1, &buffer);
}

void SetShaderResources(UINT n, ID3D11ShaderResourceView** views)
{
 if(lpDeviceContext && n && views) lpDeviceContext->PSSetShaderResources(0, n, views);
}

#pragma endregion DIRECT3D_SHADER_FUNCTIONS

#pragma region DIRECT3D_INPUT_ASSEMBLY_FUNCTIONS

void SetVertexBuffer(ID3D11Buffer* buffer, UINT stride, UINT offset)
{
 if(lpDeviceContext) lpDeviceContext->IASetVertexBuffers(0, 1, &buffer, &stride, &offset);
}

void SetVertexBufferArray(ID3D11Buffer** buffer, DWORD n, const UINT* stride, const UINT* offset)
{
 if(lpDeviceContext) lpDeviceContext->IASetVertexBuffers(0, n, buffer, stride, offset);
}

void SetIndexBuffer(ID3D11Buffer* buffer, UINT offset, DXGI_FORMAT format)
{
 if(lpDeviceContext) lpDeviceContext->IASetIndexBuffer(buffer, format, offset);
}

void SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY topology)
{
 if(lpDeviceContext) lpDeviceContext->IASetPrimitiveTopology(topology);
}

void DrawPointList(UINT vertices)
{
 if(lpDeviceContext && vertices) {
    lpDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
    lpDeviceContext->Draw(vertices, 0);
   }
}

void DrawLineList(UINT vertices)
{
 if(lpDeviceContext && vertices) {
    lpDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
    lpDeviceContext->Draw(vertices, 0);
   }
}

void DrawLineList(UINT vertices, UINT start)
{
 if(lpDeviceContext && vertices) {
    lpDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
    lpDeviceContext->Draw(vertices, start);
   }
}

void DrawPointList(UINT vertices, UINT start)
{
 if(lpDeviceContext && vertices) {
    lpDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
    lpDeviceContext->Draw(vertices, start);
   }
}

void DrawIndexedLineList(UINT indices)
{
 if(lpDeviceContext && indices) {
    lpDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
    lpDeviceContext->DrawIndexed(indices, 0, 0);
   }
}

void DrawIndexedTriangleList(UINT indices)
{
 if(lpDeviceContext && indices) {
    lpDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    lpDeviceContext->DrawIndexed(indices, 0, 0);
   }
}

#pragma endregion DIRECT3D_INPUT_ASSEMBLY_FUNCTIONS