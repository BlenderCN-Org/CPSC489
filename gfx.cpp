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

 // obtain back buffer
 ID3D11Texture2D* lpBackBuffer = NULL;
 result = lpSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&lpBackBuffer);
 if(FAILED(result)) return EC_D3D_GET_BUFFER;

 // save backbuffer descriptor (save dimensions so you can create a depth buffer later of the same size)
 D3D11_TEXTURE2D_DESC bbd;
 ZeroMemory(&bbd, sizeof(bbd));
 lpBackBuffer->GetDesc(&bbd);

 // create render target view and assign to it a back buffer
 // a render target is a collection of buffers that we are going to render to, like framebuffer objects in OpenGL
 result = lpDevice->CreateRenderTargetView(lpBackBuffer, NULL, &lpRenderTargetView);
 if(FAILED(result)) return EC_D3D_CREATE_RENDER_TARGET_VIEW;

 // release backbuffer as it is no longer needed
 lpBackBuffer->Release();
 lpBackBuffer = 0;

 // create depth buffer
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
 if(FAILED(lpDevice->CreateTexture2D(&dsd, NULL, &lpDepthTexture)))
    return EC_D3D_CREATE_TEXTURE2D;

// create depth stencil view
 D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
 ZeroMemory(&dsvd, sizeof(dsvd));
 dsvd.Format = dsd.Format;
 dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
 if(FAILED(lpDevice->CreateDepthStencilView(lpDepthTexture, &dsvd, &lpDepthStencil)))
    return EC_D3D_CREATE_DEPTH_STENCIL_VIEW;

 // set render target and assign to it a depth/stencil buffer
 // at this point we have our color, depth, and stencil buffers ready and assigned to the current render target
 lpDeviceContext->OMSetRenderTargets(1, &lpRenderTargetView, lpDepthStencil);

 // we are ready to create shaders and render now!

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
 // nothing to reset
 if(!lpDeviceContext) return EC_SUCCESS;

 // stop rendering
 lpDeviceContext->OMSetRenderTargets(0, NULL, NULL);

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

 // resize display buffers
 HRESULT result = lpSwapChain->ResizeBuffers(0, dx, dy, DXGI_FORMAT_UNKNOWN, 0);
 if(FAILED(result)) return EC_D3D_RESIZE_BUFFERS;

 // get interface to back buffer
 ID3D11Texture2D* lpBackBuffer = NULL;
 result = lpSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&lpBackBuffer);
 if(FAILED(result)) return EC_D3D_GET_BUFFER;

 // get backbuffer descriptor
 D3D11_TEXTURE2D_DESC bbd = {0};
 lpBackBuffer->GetDesc(&bbd);

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

 return EC_SUCCESS;
}

void FreeD3D(void)
{
 // release framebuffer objects
 if(lpDepthTexture) {
    lpDepthTexture->Release();
    lpDepthTexture = NULL;
   }
 if(lpDepthStencil) {
    lpDepthStencil->Release();
    lpDepthStencil = NULL;
   }
 if(lpRenderTargetView) {
    lpRenderTargetView->Release();
    lpRenderTargetView = NULL;
   }

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

BOOL RenderFrame(void)
{
 // validate
 if(!lpDeviceContext) return FALSE;
 if(!lpSwapChain) return FALSE;

 // clear buffers
 FLOAT color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
 lpDeviceContext->ClearRenderTargetView(lpRenderTargetView, color);
 lpDeviceContext->ClearDepthStencilView(lpDepthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

 // present
 lpSwapChain->Present(0, 0);

 return TRUE;
}