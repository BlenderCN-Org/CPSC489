/** \file    layouts.cpp
 *  \brief   Direct3D Input Layout functions and variables.
 *  \details An input layout map associates each input layout with a vertex shader. This is necessary because
 *           Direct3D checks the input layout with the vertex shader to make sure the signatures match. Later
 *           on, you can associate/set an input layout together with another vertex shader as long their sig-
 *           natures match.
 *  \author  Steven F. Emory
 *  \date    02/04/2018
 */

#include "stdafx.h"
#include "errors.h"
#include "app.h"
#include "win.h"
#include "gfx.h"
#include "shaders.h"
#include "layouts.h"

// Direct3D Input Layout Variables
typedef DWORD INPUT_LAYOUT_INDEX;
typedef DWORD VERTEX_SHADER_ID;
typedef std::map<INPUT_LAYOUT_INDEX, VERTEX_SHADER_ID> input_layout_map_type;
static std::deque<std::vector<D3D11_INPUT_ELEMENT_DESC>> descriptors;
static std::vector<ID3D11InputLayout*> input_layouts;
static input_layout_map_type input_layout_map;

#pragma region INPUT_LAYOUT_FUNCTIONS

ErrorCode InitInputLayouts(void)
{
 // must have device
 ID3D11Device* device = GetD3DDevice();
 if(!device) return EC_D3D_DEVICE;

 // delete previous
 FreeInputLayouts();

 // initialize indices
 DWORD IL_index = 0ul;
 DWORD VS_index = 0xFFFFFFFFul;

 // resize list of descriptors
 descriptors.resize(6);

 // INPUT LAYOUT INDEX #0
 // 1: POSITION (real32, real32, real32, real32)
 IL_index = IL_P4;
 VS_index = VS_DEFAULT;
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
 IL_index = IL_P4_C4;
 VS_index = VS_VERTEX_COLOR;
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
 IL_index = IL_P4_C4_M4_S4;
 VS_index = VS_AXES;
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
 // 1: POSITION     float4  0 + 16
 // 2: NORMAL       float4 16 + 16
 // 3: TEXCOORD1    float2 32 + 8
 // 4: TEXCOORD2    float2 40 + 8
 // 5: BLENDINDICES uint4  48 + 8
 // 6: BLENDWEIGHT  float4 56 + 16
 // 7: COLOR1       float4 72 + 16
 // 8: COLOR2       float4 88
 IL_index = IL_P4_N4_T2_T2_I4_W4_C4_C4;
 VS_index = VS_MODEL;
 input_layout_map.insert(input_layout_map_type::value_type(IL_index, VS_index));
 descriptors[IL_index] = std::vector<D3D11_INPUT_ELEMENT_DESC>(8);
 descriptors[IL_index][0].SemanticName = "POSITION";
 descriptors[IL_index][0].SemanticIndex = 0;
 descriptors[IL_index][0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
 descriptors[IL_index][0].InputSlot = 0;
 descriptors[IL_index][0].AlignedByteOffset = 0;
 descriptors[IL_index][0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
 descriptors[IL_index][0].InstanceDataStepRate = 0;
 descriptors[IL_index][1].SemanticName = "NORMAL";
 descriptors[IL_index][1].SemanticIndex = 0;
 descriptors[IL_index][1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
 descriptors[IL_index][1].InputSlot = 0;
 descriptors[IL_index][1].AlignedByteOffset = 16;
 descriptors[IL_index][1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
 descriptors[IL_index][1].InstanceDataStepRate = 0;
 descriptors[IL_index][2].SemanticName = "TEXCOORD";
 descriptors[IL_index][2].SemanticIndex = 0;
 descriptors[IL_index][2].Format = DXGI_FORMAT_R32G32_FLOAT;
 descriptors[IL_index][2].InputSlot = 0;
 descriptors[IL_index][2].AlignedByteOffset = 32;
 descriptors[IL_index][2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
 descriptors[IL_index][2].InstanceDataStepRate = 0;
 descriptors[IL_index][3].SemanticName = "TEXCOORD";
 descriptors[IL_index][3].SemanticIndex = 1;
 descriptors[IL_index][3].Format = DXGI_FORMAT_R32G32_FLOAT;
 descriptors[IL_index][3].InputSlot = 0;
 descriptors[IL_index][3].AlignedByteOffset = 40;
 descriptors[IL_index][3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
 descriptors[IL_index][3].InstanceDataStepRate = 0;
 descriptors[IL_index][4].SemanticName = "BLENDINDICES";
 descriptors[IL_index][4].SemanticIndex = 0;
 descriptors[IL_index][4].Format = DXGI_FORMAT_R16G16B16A16_UINT;
 descriptors[IL_index][4].InputSlot = 0;
 descriptors[IL_index][4].AlignedByteOffset = 48;
 descriptors[IL_index][4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
 descriptors[IL_index][4].InstanceDataStepRate = 0;
 descriptors[IL_index][5].SemanticName = "BLENDWEIGHTS";
 descriptors[IL_index][5].SemanticIndex = 0;
 descriptors[IL_index][5].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
 descriptors[IL_index][5].InputSlot = 0;
 descriptors[IL_index][5].AlignedByteOffset = 56;
 descriptors[IL_index][5].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
 descriptors[IL_index][5].InstanceDataStepRate = 0;
 descriptors[IL_index][6].SemanticName = "COLOR";
 descriptors[IL_index][6].SemanticIndex = 0;
 descriptors[IL_index][6].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
 descriptors[IL_index][6].InputSlot = 0;
 descriptors[IL_index][6].AlignedByteOffset = 72;
 descriptors[IL_index][6].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
 descriptors[IL_index][6].InstanceDataStepRate = 0;
 descriptors[IL_index][7].SemanticName = "COLOR";
 descriptors[IL_index][7].SemanticIndex = 1;
 descriptors[IL_index][7].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
 descriptors[IL_index][7].InputSlot = 0;
 descriptors[IL_index][7].AlignedByteOffset = 88;
 descriptors[IL_index][7].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
 descriptors[IL_index][7].InstanceDataStepRate = 0;

 // INPUT LAYOUT INDEX #4
 // 1: POSITION     float4  0 + 16
 // 2: CENTER       float4 16 + 16
 // 3: HALFWIDTHS   float4 32 + 16
 IL_index = IL_AABB;
 VS_index = VS_AABB;
 input_layout_map.insert(input_layout_map_type::value_type(IL_index, VS_index));
 descriptors[IL_index] = std::vector<D3D11_INPUT_ELEMENT_DESC>(3);
 descriptors[IL_index][0].SemanticName = "POSITION";
 descriptors[IL_index][0].SemanticIndex = 0;
 descriptors[IL_index][0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
 descriptors[IL_index][0].InputSlot = 0;
 descriptors[IL_index][0].AlignedByteOffset = 0;
 descriptors[IL_index][0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
 descriptors[IL_index][0].InstanceDataStepRate = 0;
 descriptors[IL_index][1].SemanticName = "CENTER";
 descriptors[IL_index][1].SemanticIndex = 0;
 descriptors[IL_index][1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
 descriptors[IL_index][1].InputSlot = 1;
 descriptors[IL_index][1].AlignedByteOffset = 0;
 descriptors[IL_index][1].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
 descriptors[IL_index][1].InstanceDataStepRate = 1;
 descriptors[IL_index][2].SemanticName = "HALFDIMS";
 descriptors[IL_index][2].SemanticIndex = 0;
 descriptors[IL_index][2].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
 descriptors[IL_index][2].InputSlot = 1;
 descriptors[IL_index][2].AlignedByteOffset = 16;
 descriptors[IL_index][2].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
 descriptors[IL_index][2].InstanceDataStepRate = 1;

 // INPUT LAYOUT INDEX #5
 // 1: POSITION     float4  0 + 16
 // 2: MINEDGE      float4 16 + 16
 // 3: MAXEDGE      float4 32 + 16
 IL_index = IL_AABB_MINMAX;
 VS_index = VS_AABB_MINMAX;
 input_layout_map.insert(input_layout_map_type::value_type(IL_index, VS_index));
 descriptors[IL_index] = std::vector<D3D11_INPUT_ELEMENT_DESC>(3);
 descriptors[IL_index][0].SemanticName = "POSITION";
 descriptors[IL_index][0].SemanticIndex = 0;
 descriptors[IL_index][0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
 descriptors[IL_index][0].InputSlot = 0;
 descriptors[IL_index][0].AlignedByteOffset = 0;
 descriptors[IL_index][0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
 descriptors[IL_index][0].InstanceDataStepRate = 0;
 descriptors[IL_index][1].SemanticName = "MINEDGE";
 descriptors[IL_index][1].SemanticIndex = 0;
 descriptors[IL_index][1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
 descriptors[IL_index][1].InputSlot = 1;
 descriptors[IL_index][1].AlignedByteOffset = 0;
 descriptors[IL_index][1].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
 descriptors[IL_index][1].InstanceDataStepRate = 1;
 descriptors[IL_index][2].SemanticName = "MINEDGE";
 descriptors[IL_index][2].SemanticIndex = 0;
 descriptors[IL_index][2].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
 descriptors[IL_index][2].InputSlot = 1;
 descriptors[IL_index][2].AlignedByteOffset = 16;
 descriptors[IL_index][2].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
 descriptors[IL_index][2].InstanceDataStepRate = 1;

 // initialize input layouts
 input_layouts.resize(descriptors.size(), nullptr);

 // create input layouts
 for(INPUT_LAYOUT_INDEX i = 0; i < input_layouts.size(); i++)
    {
     // input layout index -> vertex shader signature
     input_layout_map_type::iterator iter = input_layout_map.find(i);
     if(iter == input_layout_map.end()) {
        FreeInputLayouts();
        return EC_D3D_INPUT_LAYOUT_LOOKUP;
       }

     // input layout is not assigned to a vertex shader
     if(iter->second == 0xFFFFFFFFul) {
        input_layouts[i] = nullptr;
        continue;
       }

     // get vertex shader
     VertexShader* shader = GetVertexShader(iter->second);
     if(!shader) {
        FreeInputLayouts();
        return EC_D3D_INPUT_LAYOUT_SHADER_LOOKUP;
       }
     if(!shader->shader) {
        FreeInputLayouts();
        return EC_D3D_INPUT_LAYOUT_SHADER_SIGNATURE;
       }
     if(!shader->code.get()) {
        FreeInputLayouts();
        return EC_D3D_INPUT_LAYOUT_SHADER_SIGNATURE;
       }
     if(!shader->size) {
        FreeInputLayouts();
        return EC_D3D_INPUT_LAYOUT_SHADER_SIGNATURE;
       }

     // create input layout
     ID3D11InputLayout* lpil = nullptr;
     HRESULT result = device->CreateInputLayout(&(descriptors[i][0]), (UINT)descriptors[i].size(), shader->code.get(), shader->size, &lpil);
     if(FAILED(result)) {
        FreeInputLayouts();
        return EC_D3D_INPUT_LAYOUT_CREATE;
       }
     
     // set input layout
     input_layouts[i] = lpil;
    }

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
 if(!ptr) return EC_D3D_INPUT_LAYOUT; // index out of bounds

 // set input layout
 context->IASetInputLayout(ptr);
 return EC_SUCCESS;
}

#pragma endregion INPUT_LAYOUT_FUNCTIONS
