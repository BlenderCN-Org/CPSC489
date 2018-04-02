#include "stdafx.h"
#include "gfx.h"
#include "layouts.h"
#include "shaders.h"
#include "grid.h"

static ID3D11Buffer* vbuffer = nullptr;
static const UINT stride = 32;
static UINT size = 10;
static UINT divisions = 10;
static UINT spacing = 1;
static UINT n_vertices = 0;

ErrorCode InitGrid(void)
{
 // must have device
 ID3D11Device* device = GetD3DDevice();
 if(!device) return EC_D3D_DEVICE;

 // free previous
 FreeGrid();

 // grid extents
 FLOAT min_x = -((FLOAT)size);
 FLOAT max_x = +((FLOAT)size);
 FLOAT min_y = -((FLOAT)size);
 FLOAT max_y = +((FLOAT)size);

 // set number of vertices
 n_vertices = 8*divisions + 4;

 // create vertex data
 struct VERTEX {
  real32 x, y, z, w;
  real32 r, g, b, a;
 };
 std::unique_ptr<VERTEX[]> data(new VERTEX[n_vertices]);

 // set fixed color
 for(uint32 i = 0; i < n_vertices; i++) {
     data[i].r = (127.0f/255.0f);
     data[i].g = (127.0f/255.0f);
     data[i].b = (127.0f/255.0f);
     data[i].a = 1.0f;
    }

 // central axis
 data[0].x = min_x;
 data[0].y = 0;
 data[0].z = 0;
 data[0].w = 1.0f;
 data[1].x = max_x;
 data[1].y = 0;
 data[1].z = 0;
 data[1].w = 1.0f;

 // central axis
 data[2].x = 0;
 data[2].y = min_y;
 data[2].z = 0;
 data[2].w = 1.0f;
 data[3].x = 0;
 data[3].y = max_y;
 data[3].z = 0;
 data[3].w = 1.0f;

 // define points
 size_t index = 4;
 for(UINT i = 0; i < divisions; i++)
    {
     // X negative side
     data[index].x = min_x + i * spacing;
     data[index].y = min_y;
     data[index].z = 0;
     data[index].w = 1.0f;
     index++;
     data[index].x = min_x + i * spacing;
     data[index].y = max_y;
     data[index].z = 0;
     data[index].w = 1.0f;
     index++;

     // X positive side
     data[index].x = max_x - i * spacing;
     data[index].y = min_y;
     data[index].z = 0;
     data[index].w = 1.0f;
     index++;
     data[index].x = max_x - i * spacing;
     data[index].y = max_y;
     data[index].z = 0;
     data[index].w = 1.0f;
     index++;

     // Y negative side
     data[index].x = min_x;
     data[index].y = min_y + i * spacing;
     data[index].z = 0;
     data[index].w = 1.0f;
     index++;
     data[index].x = max_x;
     data[index].y = min_y + i * spacing;
     data[index].z = 0;
     data[index].w = 1.0f;
     index++;

     // Y positive side
     data[index].x = min_x;
     data[index].y = max_y - i * spacing;
     data[index].z = 0;
     data[index].w = 1.0f;
     index++;
     data[index].x = max_x;
     data[index].y = max_y - i * spacing;
     data[index].z = 0;
     data[index].w = 1.0f;
     index++;
    }

 // create vertex buffer
 return CreateVertexBuffer((LPVOID)data.get(), n_vertices, stride, &vbuffer);
}

void FreeGrid(void)
{
 if(vbuffer) {
    vbuffer->Release();
    vbuffer = nullptr;
   }
}

ErrorCode RenderGrid(void)
{
 // must have device context
 auto context = GetD3DDeviceContext();
 if(!context) return EC_D3D_DEVICE_CONTEXT;

 // set input layout
 ErrorCode code = SetInputLayout(IL_P4_C4);
 if(Fail(code)) return code;

 // set shaders
 code = SetVertexShader(VS_VERTEX_COLOR);
 if(Fail(code)) return code;
 code = SetPixelShader(PS_VERTEX_COLOR);
 if(Fail(code)) return code;

 // set per-model const data
 SetVertexShaderPerModelBuffer(GetIdentityMatrix());

 // set buffers
 SetVertexBuffer(vbuffer, stride, 0);

 // render
 DrawLineList(n_vertices);
 return Success();
}