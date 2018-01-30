#include "stdafx.h"
#include "errors.h"
#include "app.h"
#include "win.h"
#include "gfx.h"

// Direct3D Variables
static ID3D11Device* lpDevice;
static ID3D11DeviceContext* lpDeviceContext;
static IDXGISwapChain* lpSwapChain;
static ID3D11RenderTargetView* lpRenderTargetView;
static ID3D11Texture2D* lpDepthTexture;
static ID3D11DepthStencilView* lpDepthStencil;

ErrorCode InitD3D(void)
{
 // already exists
 if(lpDevice) return EC_SUCCESS;

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

 return EC_SUCCESS;
}

void FreeD3D(void)
{
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