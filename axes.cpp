#include "stdafx.h"
#include "errors.h"
#include "gfx.h"
#include "layouts.h"
#include "shaders.h"
#include "axes.h"

// Description:
//  In general there is only one XYZ-axes model, with x-axis red, y-axis green, 
//  and z-axis blue. When rendered, we may want to use instancing to render all
//  axes models for a particular skeleton or object.
// Notes:
//  MSDN states that drawing instanced data is faster for indexed primitives so
//  therefore we will use them here as well.

// axis buffers
static ID3D11Buffer* vbuffer = nullptr;
static ID3D11Buffer* ibuffer = nullptr;

// bounding box buffers
static ID3D11Buffer* BB_vbuffer = nullptr;
static ID3D11Buffer* BB_ibuffer = nullptr;
static const uint32 BB_vertices = 8;
static const uint32 BB_vb_stride = 16;
static const uint32 BB_id_stride = 32; // float4 (center) float4 (half-widths)
static const uint32 BB_ib_stride = 2;
static const uint32 BB_indices = 24;

ErrorCode InitAxesModel(void)
{
 // must have device
 ID3D11Device* device = GetD3DDevice();
 if(!device) return EC_D3D_DEVICE;

 // free previous
 FreeAxesModel();

 // create vertex data
 struct VERTEX {
  real32 x, y, z, w;
  real32 r, g, b, a;
 };
 uint32 n_vertices = 12;
 uint32 vb_stride = 32;
 std::unique_ptr<VERTEX[]> data(new VERTEX[n_vertices]);

 // v[0]-v[2] = origin
 data[0x0].x = 0.0f; data[0x0].y = +0.0f; data[0x0].z = +0.0f; data[0x0].w = 1.0f;
 data[0x1].x = 0.0f; data[0x1].y = +0.0f; data[0x1].z = +0.0f; data[0x1].w = 1.0f;
 data[0x2].x = 0.0f; data[0x2].y = +0.0f; data[0x2].z = +0.0f; data[0x2].w = 1.0f;
 // v[3]-v[5] = x-axis
 data[0x3].x = 1.0f; data[0x3].y = +0.0f; data[0x3].z = +0.0f; data[0x3].w = 1.0f;
 data[0x4].x = 0.9f; data[0x4].y = +0.1f; data[0x4].z = +0.0f; data[0x4].w = 1.0f;
 data[0x5].x = 0.9f; data[0x5].y = -0.1f; data[0x5].z = +0.0f; data[0x5].w = 1.0f;
 // v[6]-v[7] = y-axis
 data[0x6].y = 1.0f; data[0x6].x = +0.0f; data[0x6].z = +0.0f; data[0x6].w = 1.0f;
 data[0x7].y = 0.9f; data[0x7].x = +0.1f; data[0x7].z = +0.0f; data[0x7].w = 1.0f;
 data[0x8].y = 0.9f; data[0x8].x = -0.1f; data[0x8].z = +0.0f; data[0x8].w = 1.0f;
 // v[8]-v[B] = z-axis
 data[0x9].z = 1.0f; data[0x9].y = +0.0f; data[0x9].x = +0.0f; data[0x9].w = 1.0f;
 data[0xA].z = 0.9f; data[0xA].y = +0.1f; data[0xA].x = +0.0f; data[0xA].w = 1.0f;
 data[0xB].z = 0.9f; data[0xB].y = -0.1f; data[0xB].x = +0.0f; data[0xB].w = 1.0f;

 // v[0]-v[2] = origin
 data[0x0].r = 1.0f; data[0x0].g = 0.0f; data[0x0].b = 0.0f; data[0x0].a = 1.0f;
 data[0x1].r = 0.0f; data[0x1].g = 1.0f; data[0x1].b = 0.0f; data[0x1].a = 1.0f;
 data[0x2].r = 0.0f; data[0x2].g = 0.0f; data[0x2].b = 1.0f; data[0x2].a = 1.0f;
 // v[3]-v[5] = x-axis
 data[0x3].r = 1.0f; data[0x3].g = 0.0f; data[0x3].b = 0.0f; data[0x3].a = 1.0f;
 data[0x4].r = 1.0f; data[0x4].g = 0.0f; data[0x4].b = 0.0f; data[0x4].a = 1.0f;
 data[0x5].r = 1.0f; data[0x5].g = 0.0f; data[0x5].b = 0.0f; data[0x5].a = 1.0f;
 // v[6]-v[7] = y-axis
 data[0x6].r = 0.0f; data[0x6].g = 1.0f; data[0x6].b = 0.0f; data[0x6].a = 1.0f;
 data[0x7].r = 0.0f; data[0x7].g = 1.0f; data[0x7].b = 0.0f; data[0x7].a = 1.0f;
 data[0x8].r = 0.0f; data[0x8].g = 1.0f; data[0x8].b = 0.0f; data[0x8].a = 1.0f;
 // v[8]-v[B] = z-axis
 data[0x9].r = 0.0f; data[0x9].g = 0.0f; data[0x9].b = 1.0f; data[0x9].a = 1.0f;
 data[0xA].r = 0.0f; data[0xA].g = 0.0f; data[0xA].b = 1.0f; data[0xA].a = 1.0f;
 data[0xB].r = 0.0f; data[0xB].g = 0.0f; data[0xB].b = 1.0f; data[0xB].a = 1.0f;

 // create face data
 uint32 n_indices = 18;
 uint32 ib_stride = 2;
 uint16 indices[18] = {
  0x0, 0x3, // x-axis
  0x3, 0x4,
  0x3, 0x5,
  0x1, 0x6, // y-axis
  0x6, 0x7,
  0x6, 0x8,
  0x2, 0x9, // z-axis
  0x9, 0xA,
  0x9, 0xB
 };

 // create buffers
 ErrorCode code = CreateVertexBuffer((LPVOID)data.get(), n_vertices, vb_stride, &vbuffer);
 if(Fail(code)) {
    FreeAxesModel();
    return code;
   }
 code = CreateIndexBuffer(&indices[0], n_indices, ib_stride, &ibuffer);
 if(Fail(code)) {
    FreeAxesModel();
    return code;
   }

 return EC_SUCCESS;
}

void FreeAxesModel(void)
{
 if(ibuffer) ibuffer->Release();
 if(vbuffer) vbuffer->Release();
 ibuffer = nullptr;
 vbuffer = nullptr;
}

ErrorCode RenderAxes(ID3D11Buffer* instance, UINT n)
{
 // must have instance buffer (OK if there is nothing to do)
 if(!instance) return EC_SUCCESS;
 if(!n) return EC_SUCCESS;

 // must have device context
 auto context = GetD3DDeviceContext();
 if(!context) return EC_D3D_DEVICE_CONTEXT;

 // variables
 uint32 n_vertices = 0xC;
 uint32 vb_stride = 0x20; // float4 + float4
 uint32 id_stride = 0x50; // matrix4x4 + float4

 // two slots
 UINT strides[2] = { vb_stride, id_stride };
 UINT offsets[2] = { 0, 0 };
 ID3D11Buffer* buffers[2] = { vbuffer, instance };

 // set input layout
 ErrorCode code = SetInputLayout(IL_P4_C4_M4_S4);
 if(Fail(code)) return code;

 // set vertex shader
 code = SetVertexShader(VS_AXES);
 if(Fail(code)) return code;

 // set pixel shader
 code = SetPixelShader(PS_VERTEX_COLOR);
 if(Fail(code)) return code;

 // set input assembly
 context->IASetVertexBuffers(0, 2, buffers, strides, offsets);
 context->IASetIndexBuffer(ibuffer, DXGI_FORMAT_R16_UINT, 0);
 context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

 // render
 uint32 n_indices = 18;
 context->DrawIndexedInstanced(n_indices, n, 0, 0, 0);
 return EC_SUCCESS;
}

//
// AABB FUNCTIONS
//
#pragma region AABB_FUNCTIONS

// AABB Model Functions
ErrorCode InitAABBModel(void)
{
 // must have device
 ID3D11Device* device = GetD3DDevice();
 if(!device) return EC_D3D_DEVICE;

 // free previous
 FreeAABBModel();

 // create vertex data
 struct VERTEX { real32 x, y, z, w; };
 std::unique_ptr<VERTEX[]> data(new VERTEX[BB_vertices]);

// set vertices
 data[0].x = -1.0f; data[0].y = -1.0f; data[0].z = +1.0f; data[0].w = 1.0f; // front upper-left
 data[1].x = +1.0f; data[1].y = -1.0f; data[1].z = +1.0f; data[1].w = 1.0f; // front upper-right
 data[2].x = +1.0f; data[2].y = -1.0f; data[2].z = -1.0f; data[2].w = 1.0f; // front lower-right
 data[3].x = -1.0f; data[3].y = -1.0f; data[3].z = -1.0f; data[3].w = 1.0f; // front lower-left
 data[4].x = -1.0f; data[4].y = +1.0f; data[4].z = +1.0f; data[4].w = 1.0f; // back upper-left
 data[5].x = +1.0f; data[5].y = +1.0f; data[5].z = +1.0f; data[5].w = 1.0f; // back upper-right
 data[6].x = +1.0f; data[6].y = +1.0f; data[6].z = -1.0f; data[6].w = 1.0f; // back lower-right
 data[7].x = -1.0f; data[7].y = +1.0f; data[7].z = -1.0f; data[7].w = 1.0f; // back lower-left

 // create face data
 uint16 indices[BB_indices] = {
  0x0, 0x1, // front connections
  0x1, 0x2,
  0x2, 0x3,
  0x3, 0x0,
  0x4, 0x5, // back connections
  0x5, 0x6,
  0x6, 0x7,
  0x7, 0x4,
  0x0, 0x4, // front-to-back connections
  0x1, 0x5,
  0x2, 0x6,
  0x3, 0x7
 };

 // create buffers
 ErrorCode code = CreateVertexBuffer((LPVOID)data.get(), BB_vertices, BB_vb_stride, &BB_vbuffer);
 if(Fail(code)) {
    FreeAABBModel();
    return code;
   }
 code = CreateIndexBuffer(&indices[0], BB_indices, BB_ib_stride, &BB_ibuffer);
 if(Fail(code)) {
    FreeAABBModel();
    return code;
   }

 return EC_SUCCESS;
}

void FreeAABBModel(void)
{
 if(BB_ibuffer) BB_ibuffer->Release();
 if(BB_vbuffer) BB_vbuffer->Release();
 BB_ibuffer = nullptr;
 BB_vbuffer = nullptr;
}

ErrorCode RenderAABB(ID3D11Buffer* instance, UINT n)
{
 // must have instance buffer (OK if there is nothing to do)
 if(!instance) return EC_SUCCESS;
 if(!n) return EC_SUCCESS;

 // must have device context
 auto context = GetD3DDeviceContext();
 if(!context) return EC_D3D_DEVICE_CONTEXT;

 // two slots
 UINT strides[2] = { BB_vb_stride, BB_id_stride };
 UINT offsets[2] = { 0, 0 };
 ID3D11Buffer* buffers[2] = { BB_vbuffer, instance };

 // set input layout
 ErrorCode code = SetInputLayout(IL_AABB);
 if(Fail(code)) return code;

 // set vertex shader
 code = SetVertexShader(VS_AABB);
 if(Fail(code)) return code;

 // set pixel shader
 code = SetPixelShader(PS_CONST_COLOR);
 if(Fail(code)) return code;

 // set input assembly
 context->IASetVertexBuffers(0, 2, buffers, strides, offsets);
 context->IASetIndexBuffer(BB_ibuffer, DXGI_FORMAT_R16_UINT, 0);
 context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

 // render
 context->DrawIndexedInstanced(BB_indices, n, 0, 0, 0);
 return EC_SUCCESS;
}

#pragma endregion AABB_FUNCTIONS