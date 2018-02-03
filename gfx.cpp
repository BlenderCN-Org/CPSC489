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

// Direct3D Input Layout Variables
typedef DWORD INPUT_LAYOUT_INDEX;
typedef DWORD VERTEX_SHADER_ID;
typedef std::map<INPUT_LAYOUT_INDEX, VERTEX_SHADER_ID> input_layout_map_type;
static std::deque<std::vector<D3D11_INPUT_ELEMENT_DESC>> descriptors;
static std::vector<ID3D11InputLayout*> input_layouts;
static input_layout_map_type input_layout_map;

#pragma region DIRECT3D_INIT_FUNCTIONS

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

 // create render target (color + depth + stencil buffer)
 ErrorCode code = InitRenderTarget();
 if(Fail(code)) return code;

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
 return InitRenderTarget(dx, dy);
}

void FreeD3D(void)
{
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

#pragma region

// Vertex Shader Functions
ErrorCode InitVertexShaders(void)
{
 return EC_SUCCESS;
}

void FreeVertexShaders(void)
{
}

#pragma endregion

#pragma region INPUT_LAYOUT_FUNCTIONS

ErrorCode InitInputLayouts(void)
{
 // static const DWORD IL_DEFAULT  = 0; // P4 (position + default color)
 // static const DWORD IL_GRID     = 1; // P4 + C4 (position + vertex color)
 // static const DWORD IL_SKELETON = 1; // P4 + C4 (position + vertex color)
 // static const DWORD IL_AXES     = 2; // P4 + C4 + M4 (position + vertex color + instance data)
 // static const DWORD IL_MODEL1   = 3; // P4 + UV2 (position + uv)

 // must have device
 ID3D11Device* device = GetD3DDevice();
 if(!device) return EC_D3D_DEVICE;

 // delete previous
 FreeInputLayouts();

 // initialize indices
 DWORD IL_index = 0ul;
 DWORD VS_index = 0xFFFFFFFFul;

 // resize list of descriptors
 descriptors.resize(2);

 // INPUT LAYOUT INDEX #0
 // 1: POSITION (real32, real32, real32, real32)
 IL_index = 0;
 VS_index = 0;
 input_layout_map.insert(input_layout_map_type::value_type(IL_index, VS_index));
 descriptors[IL_index] = std::vector<D3D11_INPUT_ELEMENT_DESC>(1);
 descriptors[IL_index][0].SemanticName = "POSITION";
 descriptors[IL_index][0].SemanticIndex = 0;
 descriptors[IL_index][0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
 descriptors[IL_index][0].InputSlot = 0;
 descriptors[IL_index][0].AlignedByteOffset = 0;
 descriptors[IL_index][0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
 descriptors[IL_index][0].InstanceDataStepRate = 0;

 // INPUT LAYOUT INDEX #1
 // 1: POSITION (real32, real32, real32, real32)
 // 2: COLOR (real32, real32, real32, real32)
 IL_index = 1;
 VS_index = 1;
 input_layout_map.insert(input_layout_map_type::value_type(IL_index, VS_index));
 descriptors[IL_index] = std::vector<D3D11_INPUT_ELEMENT_DESC>(2);
 descriptors[IL_index][0].SemanticName = "POSITION";
 descriptors[IL_index][0].SemanticIndex = 0;
 descriptors[IL_index][0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
 descriptors[IL_index][0].InputSlot = 0;
 descriptors[IL_index][0].AlignedByteOffset = 0;
 descriptors[IL_index][0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
 descriptors[IL_index][0].InstanceDataStepRate = 0;
 descriptors[IL_index][1].SemanticName = "COLOR";
 descriptors[IL_index][1].SemanticIndex = 0;
 descriptors[IL_index][1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
 descriptors[IL_index][1].InputSlot = 0;
 descriptors[IL_index][1].AlignedByteOffset = 16;
 descriptors[IL_index][1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
 descriptors[IL_index][1].InstanceDataStepRate = 0;

 // INPUT LAYOUT INDEX #2
 // 1: POSITION  (real32, real32, real32, real32)
 // 2: COLOR     (real32, real32, real32, real32)
 // 3: TRANSFORM (real32, real32, real32, real32) matrix row 1
 // 4: TRANSFORM (real32, real32, real32, real32) matrix row 2
 // 5: TRANSFORM (real32, real32, real32, real32) matrix row 3
 // 6: TRANSFORM (real32, real32, real32, real32) matrix row 4
 // 7: SCALE     (real32, real32, real32, real32) scale
 IL_index = 2;
 VS_index = 2;
 input_layout_map.insert(input_layout_map_type::value_type((INPUT_LAYOUT_INDEX)IL_index, VS_index));
 descriptors[IL_index] = std::vector<D3D11_INPUT_ELEMENT_DESC>(7);
 descriptors[IL_index][0].SemanticName = "POSITION";
 descriptors[IL_index][0].SemanticIndex = 0;
 descriptors[IL_index][0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
 descriptors[IL_index][0].InputSlot = 0;
 descriptors[IL_index][0].AlignedByteOffset = 0;
 descriptors[IL_index][0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
 descriptors[IL_index][0].InstanceDataStepRate = 0;
 descriptors[IL_index][1].SemanticName = "COLOR";
 descriptors[IL_index][1].SemanticIndex = 0;
 descriptors[IL_index][1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
 descriptors[IL_index][1].InputSlot = 0;
 descriptors[IL_index][1].AlignedByteOffset = 16;
 descriptors[IL_index][1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
 descriptors[IL_index][1].InstanceDataStepRate = 0;
 descriptors[IL_index][2].SemanticName = "TRANSFORM";
 descriptors[IL_index][2].SemanticIndex = 0;
 descriptors[IL_index][2].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
 descriptors[IL_index][2].InputSlot = 1;
 descriptors[IL_index][2].AlignedByteOffset = 0;
 descriptors[IL_index][2].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
 descriptors[IL_index][2].InstanceDataStepRate = 1;
 descriptors[IL_index][3].SemanticName = "TRANSFORM";
 descriptors[IL_index][3].SemanticIndex = 1;
 descriptors[IL_index][3].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
 descriptors[IL_index][3].InputSlot = 1;
 descriptors[IL_index][3].AlignedByteOffset = 16;
 descriptors[IL_index][3].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
 descriptors[IL_index][3].InstanceDataStepRate = 1;
 descriptors[IL_index][4].SemanticName = "TRANSFORM";
 descriptors[IL_index][4].SemanticIndex = 2;
 descriptors[IL_index][4].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
 descriptors[IL_index][4].InputSlot = 1;
 descriptors[IL_index][4].AlignedByteOffset = 32;
 descriptors[IL_index][4].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
 descriptors[IL_index][4].InstanceDataStepRate = 1;
 descriptors[IL_index][5].SemanticName = "TRANSFORM";
 descriptors[IL_index][5].SemanticIndex = 3;
 descriptors[IL_index][5].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
 descriptors[IL_index][5].InputSlot = 1;
 descriptors[IL_index][5].AlignedByteOffset = 48;
 descriptors[IL_index][5].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
 descriptors[IL_index][5].InstanceDataStepRate = 1;
 descriptors[IL_index][6].SemanticName = "SCALE";
 descriptors[IL_index][6].SemanticIndex = 0;
 descriptors[IL_index][6].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
 descriptors[IL_index][6].InputSlot = 1;
 descriptors[IL_index][6].AlignedByteOffset = 64;
 descriptors[IL_index][6].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
 descriptors[IL_index][6].InstanceDataStepRate = 1;

 // INPUT LAYOUT INDEX #3
 // 1: POSITION (real32, real32, real32, real32)
 // 2: TEXCOORD (real32, real32)
 IL_index = 3;
 VS_index = 3;
 input_layout_map.insert(input_layout_map_type::value_type((INPUT_LAYOUT_INDEX)IL_index, VS_index)); // not assigned
 descriptors[IL_index] = std::vector<D3D11_INPUT_ELEMENT_DESC>(2);
 descriptors[IL_index][0].SemanticName = "POSITION";
 descriptors[IL_index][0].SemanticIndex = 0;
 descriptors[IL_index][0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
 descriptors[IL_index][0].InputSlot = 0;
 descriptors[IL_index][0].AlignedByteOffset = 0;
 descriptors[IL_index][0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
 descriptors[IL_index][0].InstanceDataStepRate = 0;
 descriptors[IL_index][1].SemanticName = "TEXCOORD";
 descriptors[IL_index][1].SemanticIndex = 0;
 descriptors[IL_index][1].Format = DXGI_FORMAT_R32G32_FLOAT;
 descriptors[IL_index][1].InputSlot = 1;
 descriptors[IL_index][1].AlignedByteOffset = 0;
 descriptors[IL_index][1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
 descriptors[IL_index][1].InstanceDataStepRate = 0;

 return EC_SUCCESS;
}

void FreeInputLayouts(void)
{
 descriptors.clear();
 input_layout_map.clear();
 for(size_t i = 0; i < input_layouts.size(); i++) {
     if(input_layouts[i]) {
        input_layouts[i]->Release();
        input_layouts[i] = 0;
       }
    }
 input_layouts.clear();
}

ID3D11InputLayout* GetInputLayout(size_t index)
{
 if(!(index < input_layouts.size())) return nullptr;
 return input_layouts[index];
}

ErrorCode SetInputLayout(size_t index)
{
 // must have device context
 ID3D11DeviceContext* context = GetD3DDeviceContext();
 if(!context) return EC_D3D_DEVICE_CONTEXT;

 // get input layout
 auto ptr = GetInputLayout(index);
 if(!ptr) return EC_D3D_GET_INPUT_LAYOUT; // index out of bounds

 // set input layout
 context->IASetInputLayout(ptr);
 return EC_SUCCESS;
}

#pragma endregion INPUT_LAYOUT_FUNCTIONS

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

 // present
 lpSwapChain->Present(0, 0);

 return TRUE;
}

#pragma endregion RENDERING_FUNCTIONS