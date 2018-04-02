#include "stdafx.h"
#include "stdgfx.h"
#include "gfx.h"
#include "camera.h"
#include "layouts.h"
#include "shaders.h"
#include "orbit.h"

static ID3D11Buffer* vbuffer = nullptr;
static ID3D11Buffer* ibuffer = nullptr;
static uint32 n_vertices = 8;
static uint32 vb_stride = 32;
static uint32 n_indices = 24;
static uint32 ib_stride = 2;
static bool scale_box = false;

ErrorCode InitOrbitBox(void)
{
 // must have device
 auto device = GetD3DDevice();
 if(!device) return EC_D3D_DEVICE;

 // camera parameters
 auto orbitcam = GetOrbitCamera();
 const real32* E = orbitcam->GetCameraOrigin();
 const real32* X = orbitcam->GetCameraXAxis();
 real32 orbit_distance = orbitcam->GetOrbitDistance();

 // compute orbit point
 real32 orbit_point[3];
 orbit_point[0] = E[0] + orbit_distance*X[0];
 orbit_point[1] = E[1] + orbit_distance*X[1];
 orbit_point[2] = E[2] + orbit_distance*X[2];

 // free previous
 FreeOrbitBox();

 // create vertex data
 struct VERTEX {
  real32 x, y, z, w;
  real32 r, g, b, a;
 };
 std::unique_ptr<VERTEX[]> data(new VERTEX[n_vertices]);

 // set fixed values
 for(uint32 i = 0; i < n_vertices; i++) {
     // position
     data[i].w = 1.0f;
     // color
     data[i].r = 1.0f;
     data[i].g = 0.0f;
     data[i].b = 0.0f;
     data[i].a = 1.0f;
    }

 // constants
 float halfsize = (scale_box ? (0.025f * orbit_distance) : 0.025f); // scale by orbit distance?
 float point[] = { 0.0f, 0.0f, 0.0f };
 float min_p[3] = { point[0] - halfsize, point[1] - halfsize, point[2] - halfsize };
 float max_p[3] = { point[0] + halfsize, point[1] + halfsize, point[2] + halfsize };

 // points at min-z
 data[0].x = min_p[0]; data[0].y = min_p[1]; data[0].z = min_p[2];
 data[1].x = min_p[0]; data[1].y = max_p[1]; data[1].z = min_p[2];
 data[2].x = max_p[0]; data[2].y = max_p[1]; data[2].z = min_p[2];
 data[3].x = max_p[0]; data[3].y = min_p[1]; data[3].z = min_p[2];

 // points at max-z
 data[4].x = min_p[0]; data[4].y = min_p[1]; data[4].z = max_p[2];
 data[5].x = min_p[0]; data[5].y = max_p[1]; data[5].z = max_p[2];
 data[6].x = max_p[0]; data[6].y = max_p[1]; data[6].z = max_p[2];
 data[7].x = max_p[0]; data[7].y = min_p[1]; data[7].z = max_p[2];

 // create vertex buffer
 ErrorCode code = CreateDynamicVertexBuffer((LPVOID)data.get(), n_vertices, vb_stride, &vbuffer);
 if(Fail(code)) {
    FreeOrbitBox();
    return code;
   }

 // create face data
 uint16 indices[24] = {
  0x0, 0x1, // min-z side
  0x1, 0x2,
  0x2, 0x3,
  0x3, 0x0,
  0x4, 0x5, // max-z side
  0x5, 0x6,
  0x6, 0x7,
  0x7, 0x4,
  0x0, 0x4, // connectors
  0x1, 0x5,
  0x2, 0x6,
  0x3, 0x7,
 };

 // create index buffer (that never changes)
 code = CreateImmutableIndexBuffer(&indices[0], n_indices, ib_stride, &ibuffer);
 if(Fail(code)) {
    FreeOrbitBox();
    return code;
   }

 return EC_SUCCESS;
}

void FreeOrbitBox(void)
{
 if(ibuffer) ibuffer->Release();
 if(vbuffer) vbuffer->Release();
 ibuffer = nullptr;
 vbuffer = nullptr;
}

ErrorCode RenderOrbitBox(void)
{
 // must have device context
 auto context = GetD3DDeviceContext();
 if(!context) return EC_D3D_DEVICE_CONTEXT;

 // set input layout
 ErrorCode code = SetInputLayout(IL_P4_C4);
 if(Fail(code)) return code;

 // set vertex shader
 code = SetVertexShader(VS_VERTEX_COLOR);
 if(Fail(code)) return code;

 // set pixel shader (use grid shader)
 code = SetPixelShader(PS_VERTEX_COLOR);
 if(Fail(code)) return code;

 // set per-model const data
 SetVertexShaderPerModelBuffer(GetIdentityMatrix());

 // set buffers
 SetVertexBuffer(vbuffer, vb_stride, 0);
 SetIndexBuffer(ibuffer, 0, DXGI_FORMAT_R16_UINT);

 // render
 DrawIndexedLineList(n_indices);
 return EC_SUCCESS;
}

ErrorCode UpdateOrbitBox(void)
{
 // must have vertex buffer, return success if nothing to do
 if(!vbuffer) return EC_SUCCESS;

 // must have device context
 auto context = GetD3DDeviceContext();
 if(!context) return EC_D3D_DEVICE_CONTEXT;

 // camera parameters
 auto orbitcam = GetOrbitCamera();
 const real32* E = orbitcam->GetCameraOrigin();
 const real32* X = orbitcam->GetCameraXAxis();
 real32 orbit_distance = orbitcam->GetOrbitDistance();

 // compute orbit point
 real32 orbit_point[3];
 orbit_point[0] = E[0] + orbit_distance*X[0];
 orbit_point[1] = E[1] + orbit_distance*X[1];
 orbit_point[2] = E[2] + orbit_distance*X[2];

 // update mapped vertex buffer data
 D3D11_MAPPED_SUBRESOURCE msr;
 ZeroMemory(&msr, sizeof(msr));
 if(!FAILED(context->Map(vbuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr)))
   {
    // vertex data
    struct VERTEX {
     real32 x, y, z, w;
     real32 r, g, b, a;
    };
    VERTEX* data = reinterpret_cast<VERTEX*>(msr.pData);

    // set fixed values (we must do this because D3D11_MAP_WRITE_DISCARD discards previous data)
    for(uint32 i = 0; i < n_vertices; i++) {
        // color
        data[i].r = 1.0f;
        data[i].g = 0.0f;
        data[i].b = 0.0f;
        data[i].a = 1.0f;
        // position
        data[i].w = 1.0f;
       }

    // constants
    float halfsize = (scale_box ? (0.025f * orbit_distance) : 0.025f); // scale by orbit distance?
    float point[] = { 0.0f, 0.0f, 0.0f };
    float min_p[3] = { orbit_point[0] - halfsize, orbit_point[1] - halfsize, orbit_point[2] - halfsize };
    float max_p[3] = { orbit_point[0] + halfsize, orbit_point[1] + halfsize, orbit_point[2] + halfsize };

    // points at min-z
    data[0].x = min_p[0]; data[0].y = min_p[1]; data[0].z = min_p[2];
    data[1].x = min_p[0]; data[1].y = max_p[1]; data[1].z = min_p[2];
    data[2].x = max_p[0]; data[2].y = max_p[1]; data[2].z = min_p[2];
    data[3].x = max_p[0]; data[3].y = min_p[1]; data[3].z = min_p[2];

    // points at max-z
    data[4].x = min_p[0]; data[4].y = min_p[1]; data[4].z = max_p[2];
    data[5].x = min_p[0]; data[5].y = max_p[1]; data[5].z = max_p[2];
    data[6].x = max_p[0]; data[6].y = max_p[1]; data[6].z = max_p[2];
    data[7].x = max_p[0]; data[7].y = min_p[1]; data[7].z = max_p[2];

    context->Unmap(vbuffer, 0);
   }

 return EC_SUCCESS;
}

bool GetOrbitBoxScale(void)
{
 return scale_box;
}

ErrorCode SetOrbitBoxScale(bool state)
{
 ::scale_box = state;
 return UpdateOrbitBox();
}