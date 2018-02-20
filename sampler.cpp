#include "stdafx.h"
#include "errors.h"
#include "app.h"
#include "gfx.h"
#include "sampler.h"

//
// SAMPLER STATE VARIABLES
//

static std::deque<ID3D11SamplerState*> samplers;

//
// SAMPLER STATE FUNCTIONS
//

ErrorCode InitSamplerStates(void)
{
 // must have device
 ID3D11Device* device = GetD3DDevice();
 if(!device) return DebugErrorCode(EC_D3D_DEVICE, __LINE__, __FILE__);

 // delete previous
 FreeSamplerStates();

 // sampler descriptors
 std::deque<D3D11_SAMPLER_DESC> sdlist;

 // SAMPLER #00
 // STANDARD LINEAR SAMPLER WITH MIPMAPPING
 sdlist.push_back(D3D11_SAMPLER_DESC());
 ZeroMemory(&sdlist.back(), sizeof(D3D11_SAMPLER_DESC));
 sdlist.back().Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
 sdlist.back().AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
 sdlist.back().AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
 sdlist.back().AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
 sdlist.back().ComparisonFunc = D3D11_COMPARISON_NEVER;
 sdlist.back().MinLOD = 0;
 sdlist.back().MaxLOD = D3D11_FLOAT32_MAX;

 // create samplers
 for(size_t i = 0; i < sdlist.size(); i++) {
     ID3D11SamplerState* ptr = NULL;
     HRESULT result = device->CreateSamplerState(&sdlist[i], &ptr);
     if(FAILED(result)) return DebugErrorCode(EC_D3D_SAMPLER_STATE_CREATE, __LINE__, __FILE__);
     samplers.push_back(ptr);
    }

 // success
 return EC_SUCCESS;
}

void FreeSamplerStates(void)
{
 for(size_t i = 0; i < samplers.size(); i++) {
     if(samplers[i]) {
        samplers[i]->Release();
        samplers[i] = 0;
       }
    }
 samplers.clear();
}

ID3D11SamplerState* GetSamplerState(DWORD index)
{
 if(!(index < samplers.size())) return nullptr;
 return samplers[index];
}

ErrorCode SetSamplerState(DWORD id)
{
 // must have device context
 ID3D11DeviceContext* context = GetD3DDeviceContext();
 if(!context) return DebugErrorCode(EC_D3D_DEVICE_CONTEXT, __LINE__, __FILE__);

 // set sampler
 ID3D11SamplerState* sampler = GetSamplerState(id);
 if(!sampler) return DebugErrorCode(EC_D3D_SAMPLER_STATE, __LINE__, __FILE__);
 context->PSSetSamplers(0, 1, &sampler);

 // success
 return EC_SUCCESS;
}

ErrorCode SetSamplerState(const DWORD* slots, DWORD n)
{
 // must have device context
 ID3D11DeviceContext* context = GetD3DDeviceContext();
 if(!context) return DebugErrorCode(EC_D3D_DEVICE_CONTEXT, __LINE__, __FILE__);

 // validate arguments
 if(!(n < D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT))
    return DebugErrorCode(EC_D3D_SAMPLER_STATE, __LINE__, __FILE__);

 // set samplers
 if(slots && n)  {
    ID3D11SamplerState* slist[D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT];
    for(UINT i = 0; i < n; i++) {
        slist[i] = GetSamplerState(slots[i]);
        if(!slist[i]) return DebugErrorCode(EC_D3D_SAMPLER_STATE, __LINE__, __FILE__);
       }
    context->PSSetSamplers(0, n, &slist[0]);
   }
 else if(!n) context->PSSetSamplers(0, 0, nullptr);
 else return DebugErrorCode(EC_D3D_SAMPLER_STATE, __LINE__, __FILE__);

 // success
 return EC_SUCCESS;
}