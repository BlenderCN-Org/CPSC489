#include "stdafx.h"
#include "app.h"
#include "gfx.h"
#include "rasters.h"

#pragma region RASTERIZER_STATE_VARIABLES

static std::deque<ID3D11RasterizerState*> rslist;

#pragma endregion RASTERIZER_STATE_VARIABLES

#pragma region RASTERIZER_STATE_FUNCTIONS

ErrorCode InitRasterizerStates(void)
{
 // must have device
 ID3D11Device* device = GetD3DDevice();
 if(!device) return DebugErrorCode(EC_D3D_DEVICE, __LINE__, __FILE__);

 // delete previous
 FreeRasterizerStates();

 // rasterizer state descriptors
 std::deque<D3D11_RASTERIZER_DESC> rsdlist;

 // RASTERIZER STATE INDEX #0
 // WIREFRAME MODELS
 rsdlist.push_back(D3D11_RASTERIZER_DESC());
 rsdlist.back().FillMode = D3D11_FILL_WIREFRAME;
 rsdlist.back().CullMode = D3D11_CULL_NONE;
 rsdlist.back().FrontCounterClockwise = TRUE;
 rsdlist.back().DepthBias = 0;
 rsdlist.back().DepthBiasClamp = 0.0f;
 rsdlist.back().SlopeScaledDepthBias = 0.0f;
 rsdlist.back().DepthClipEnable = TRUE;
 rsdlist.back().ScissorEnable = FALSE;
 rsdlist.back().MultisampleEnable = FALSE;
 rsdlist.back().AntialiasedLineEnable = FALSE;

 // RASTERIZER STATE INDEX #1
 // SOLID MODELS
 rsdlist.push_back(D3D11_RASTERIZER_DESC());
 rsdlist.back().FillMode = D3D11_FILL_SOLID;
 rsdlist.back().CullMode = D3D11_CULL_BACK;
 rsdlist.back().FrontCounterClockwise = TRUE;
 rsdlist.back().DepthBias = 0;
 rsdlist.back().DepthBiasClamp = 0.0f;
 rsdlist.back().SlopeScaledDepthBias = 0.0f;
 rsdlist.back().DepthClipEnable = TRUE;
 rsdlist.back().ScissorEnable = FALSE;
 rsdlist.back().MultisampleEnable = FALSE;
 rsdlist.back().AntialiasedLineEnable = FALSE;

 // create rasterizer state list
 for(size_t i = 0; i < rsdlist.size(); i++) {
     ID3D11RasterizerState* lprs = nullptr;
     HRESULT result = device->CreateRasterizerState(&rsdlist[i], &lprs);
     if(FAILED(result)) return DebugErrorCode(EC_D3D_RASTERIZER_STATE_CREATE, __LINE__, __FILE__);
     rslist.push_back(lprs);
    }

 return EC_SUCCESS;
}

void FreeRasterizerStates(void)
{
 for(size_t i = 0; i < rslist.size(); i++) {
     if(rslist[i]) {
        rslist[i]->Release();
        rslist[i] = 0;
       }
    }
 rslist.clear();
}

ID3D11RasterizerState* GetRasterizerState(DWORD index)
{
 if(!(index < rslist.size())) return nullptr;
 return rslist[index];
}

ErrorCode SetRasterizerState(DWORD index)
{
 // must have device context
 ID3D11DeviceContext* context = GetD3DDeviceContext();
 if(!context) return DebugErrorCode(EC_D3D_DEVICE_CONTEXT, __LINE__, __FILE__);

 // set state
 ID3D11RasterizerState* lprs = GetRasterizerState(index);
 if(!lprs) return DebugErrorCode(EC_D3D_RASTERIZER_STATE, __LINE__, __FILE__);
 context->RSSetState(lprs);

 // success
 return EC_SUCCESS;
}

#pragma endregion RASTERIZER_STATE_FUNCTIONS