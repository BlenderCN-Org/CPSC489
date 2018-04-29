#include "../stdafx.h"
#include "../stdgfx.h"
#include "../errors.h"
#include "../app.h"
#include "../aabb.h"
#include "../model.h"
#include "../camera.h"
#include "../xinput.h"
#include "../gfx.h"
#include "../axes.h"
#include "tests.h"
#include "t_minmax.h"

// buffers
static real32 color[4] = { 0.0f, 0.4f, 0.9f, 1.0f };
static ID3D11Buffer* color_buffer = nullptr;
static ID3D11Buffer* instance_buffer = nullptr;
static const uint32 n_aabb = 2;

// don't worry, BeginTest() calls free function on fail.
BOOL InitAABBMinMaxTest(void)
{
 // create AABBs
 AABB_minmax data[n_aabb];

 data[0].a[0] = 1.0f; data[0].b[0] = 2.0f;
 data[0].a[1] = 1.0f; data[0].b[1] = 2.0f;
 data[0].a[2] = 1.0f; data[0].b[2] = 2.0f;
 data[0].a[3] = 1.0f; data[0].b[3] = 1.0f;

 data[1].a[0] = 1.0f; data[1].b[0] = 3.0f;
 data[1].a[1] = 1.0f; data[1].b[1] = 3.0f;
 data[1].a[2] = 1.0f; data[1].b[2] = 3.0f;
 data[1].a[3] = 1.0f; data[1].b[3] = 1.0f;

 // create color buffer
 ErrorCode code = CreateImmutableFloat4ConstBuffer(color, &color_buffer);
 if(Fail(code)) {
    DebugErrorCode(code, __LINE__, __FILE__);
    return FALSE;
   }

 // create instance buffer
 code = CreateImmutableConstBuffer(&instance_buffer, n_aabb*sizeof(AABB_halfdim), &data[0]);
 if(Fail(code)) {
    DebugErrorCode(code, __LINE__, __FILE__);
    return FALSE;
   }

 return TRUE;
}

void FreeAABBMinMaxTest(void)
{
 if(color_buffer) color_buffer->Release();
 if(instance_buffer) instance_buffer->Release();
 color_buffer = nullptr;
 instance_buffer = nullptr;
}

void UpdateAABBMinMaxTest(real32 dt)
{
}

void RenderAABBMinMaxTest(void)
{
 // set per-model buffers and render
 SetVertexShaderPerModelBuffer(GetIdentityMatrix());
 if(color_buffer) SetPixelShaderPerModelBuffer(color_buffer);
 if(instance_buffer) RenderAABBMinMax(instance_buffer, n_aabb);
}