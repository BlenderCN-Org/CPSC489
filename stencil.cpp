#include "stdafx.h"
#include "errors.h"
#include "app.h"
#include "win.h"
#include "gfx.h"
#include "stencil.h"

static std::vector<ID3D11DepthStencilState*> stencils;

ErrorCode InitStencilStates(void)
{
 // must have device
 ID3D11Device* device = GetD3DDevice();
 if(!device) return EC_D3D_DEVICE;

 D3D11_DEPTH_STENCIL_DESC dsd;
 ID3D11DepthStencilState* dss = nullptr;

 // #0
 // draw the skeleton first
 // stencil testing ALWAYS passes
 // if depth testing fails, keep previous stencil value
 // don't do any backface testing
 ZeroMemory(&dsd, sizeof(dsd));
 dsd.DepthEnable = TRUE;
 dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
 dsd.DepthFunc = D3D11_COMPARISON_LESS;
 dsd.StencilEnable = TRUE;
 dsd.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
 dsd.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
 dsd.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
 dsd.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
 dsd.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
 dsd.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
 dsd.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
 dsd.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
 dsd.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
 dsd.BackFace.StencilFunc = D3D11_COMPARISON_NEVER;
 if(FAILED(device->CreateDepthStencilState(&dsd, &dss))) return DebugErrorCode(EC_D3D_STENCIL_STATE_CREATE, __LINE__, __FILE__);
 stencils.push_back(dss);
 dss = nullptr;

 // #1
 // draw the model second
 // we do not need to write to the stencil buffer, all we need to do is read the stencil buffer
 ZeroMemory(&dsd, sizeof(dsd));
 dsd.DepthEnable = TRUE;
 dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
 dsd.DepthFunc = D3D11_COMPARISON_LESS;
 dsd.StencilEnable = TRUE;
 dsd.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
 dsd.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
 // if not over skeleton, ref(model) = 1 and ref(skeleton) = 0,
 // so stencil function passes and pixel is overwritten
 dsd.FrontFace.StencilFunc = D3D11_COMPARISON_NOT_EQUAL;
 dsd.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
 dsd.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
 dsd.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
 // not important
 dsd.BackFace.StencilFunc = D3D11_COMPARISON_NEVER;
 dsd.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
 dsd.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
 dsd.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
 if(FAILED(device->CreateDepthStencilState(&dsd, &dss))) return DebugErrorCode(EC_D3D_STENCIL_STATE_CREATE, __LINE__, __FILE__);
 stencils.push_back(dss);
 dss = nullptr;

 // #2
 // stenciled model (read:yes write:no alpha:yes)
 // we do not need to write to the stencil buffer, all we need to do is read the stencil buffer
 ZeroMemory(&dsd, sizeof(dsd));
 dsd.DepthEnable = TRUE;
 dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
 dsd.DepthFunc = D3D11_COMPARISON_LESS;
 dsd.StencilEnable = TRUE;
 dsd.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
 dsd.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
 // if not over skeleton, ref(model) = 1 and ref(skeleton) = 0,
 // so stencil function passes and pixel is overwritten
 dsd.FrontFace.StencilFunc = D3D11_COMPARISON_NOT_EQUAL;
 dsd.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
 dsd.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
 dsd.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
 // not important
 dsd.BackFace.StencilFunc = D3D11_COMPARISON_NEVER;
 dsd.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
 dsd.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
 dsd.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
 if(FAILED(device->CreateDepthStencilState(&dsd, &dss))) return DebugErrorCode(EC_D3D_STENCIL_STATE_CREATE, __LINE__, __FILE__);
 stencils.push_back(dss);
 dss = nullptr;

 return EC_SUCCESS;
}

void FreeStencilStates(void)
{
 for(uint32 i = 0; i < stencils.size(); i++) {
     stencils[i]->Release();
     stencils[i] = nullptr;
    }
}

ID3D11DepthStencilState* GetStencilState(UINT index)
{
 if(!(index < stencils.size())) return nullptr;
 return stencils[index];
}

ErrorCode SetStencilState(UINT index, UINT value)
{
 ID3D11DeviceContext* context = GetD3DDeviceContext();
 if(!context) return DebugErrorCode(EC_D3D_DEVICE_CONTEXT, __LINE__, __FILE__);
 if(!(index < stencils.size())) return DebugErrorCode(EC_D3D_STENCIL_STATE, __LINE__, __FILE__);
 context->OMSetDepthStencilState(stencils[index], value);
 return EC_SUCCESS;
}

ErrorCode ResetStencilState(void)
{
 ID3D11DeviceContext* context = GetD3DDeviceContext();
 if(!context) return DebugErrorCode(EC_D3D_DEVICE_CONTEXT, __LINE__, __FILE__);
 context->OMSetDepthStencilState(nullptr, 0);
 return EC_SUCCESS;
}