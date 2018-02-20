#include "stdafx.h"
#include "errors.h"
#include "gfx.h"
#include "blending.h"

static std::deque<ID3D11BlendState*> bslist;

ErrorCode InitBlendStates(void)
{
 // must have device
 auto device = GetD3DDevice();
 if(!device) return DebugErrorCode(EC_D3D_DEVICE, __LINE__, __FILE__);

 // create blend state #0
 // NULL is the default blend state
 bslist.push_back(nullptr);

 // blend state #1
 // C = C(src) x F(src) op C(dst) x F(dst)
 // A = A(src) x F(src) op A(dst) x F(dst)
 // source = current pixel
 // destination = pixel underneath, about to be drawn over
 // src(r, g, b) if src(a) = 1
 // dst(r, g, b) if src(a) = 0
 // <sr, sg, sb> x <sa, sa, sa> 
 // <dr, dg, db> x <1 - sa, 1 - sa, 1 - sa> = <0, 0, 0> if sa = 1
 // say S.r = 0.4
 // say S.a = 0.5
 // say D.r = 0.8
 // so 0.4 + 0.5*(1.0 - 0.8)
 // we want
 // r = S.r + D.r*(1 - S.a)

 // blend state descriptor #1
 D3D11_BLEND_DESC bd;
 ZeroMemory(&bd, sizeof(bd));
 bd.AlphaToCoverageEnable = FALSE;
 bd.RenderTarget[0].BlendEnable = TRUE;
 bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
 bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
 bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
 bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
 bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
 bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
 bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

 // create blend state #1
 ID3D11BlendState* bs;
 if(FAILED(device->CreateBlendState(&bd, &bs))) {
    FreeBlendStates();
    return DebugErrorCode(EC_D3D_BLEND_STATE_CREATE, __LINE__, __FILE__);
   }
 bslist.push_back(bs);

 // success
 return EC_SUCCESS;
}

void FreeBlendStates(void)
{
 for(size_t i = 0; i < bslist.size(); i++) {
     if(bslist[i]) bslist[i]->Release();
     bslist[i] = nullptr;
    }
 bslist.clear();
}

ID3D11BlendState* GetBlendState(DWORD index)
{
 if(!(index < bslist.size())) return nullptr;
 return bslist[index];
}

ErrorCode SetBlendState(DWORD index)
{
 auto context = GetD3DDeviceContext();
 if(!context) return DebugErrorCode(EC_D3D_DEVICE_CONTEXT, __LINE__, __FILE__);
 float factor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
 if(!(index < bslist.size())) return DebugErrorCode(EC_D3D_BLEND_STATE, __LINE__, __FILE__);
 context->OMSetBlendState(bslist[index], factor, 0xFFFFFFFFul);
 return EC_SUCCESS;
}

ErrorCode SetBlendState(DWORD index, const float* factor)
{
 auto context = GetD3DDeviceContext();
 if(!context) return DebugErrorCode(EC_D3D_DEVICE_CONTEXT, __LINE__, __FILE__);
 context->OMSetBlendState(bslist[index], factor, 0xFFFFFFFFul);
 return EC_SUCCESS;
}