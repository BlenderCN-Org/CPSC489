#include "../stdafx.h"
#include "../stdgfx.h"
#include "../errors.h"
#include "../app.h"
#include "../math.h"
#include "../vector3.h"
#include "../matrix4.h"
#include "../model.h"
#include "../camera.h"
#include "../xinput.h"
#include "../gfx.h"
#include "../axes.h"
#include "tests.h"
#include "t_aabb.h"

struct AABB_halfdim {
 real32 position[4];
 real32 halfdims[4];
};

// buffers
static real32 color[4] = { 0.0f, 0.4f, 0.9f, 1.0f };
static ID3D11Buffer* color_buffer = nullptr;
static ID3D11Buffer* instance_buffer = nullptr;
static const uint32 n_aabb = 2;

// don't worry, BeginTest() calls free function on fail.
BOOL InitAABBTest(void)
{
 // create AABBs
 AABB_halfdim data[n_aabb];
 data[0].position[0] = 1.0f; data[1].position[0] = 2.0f;
 data[0].position[1] = 1.0f; data[1].position[1] = 2.0f;
 data[0].position[2] = 1.0f; data[1].position[2] = 2.0f;
 data[0].position[3] = 1.0f; data[1].position[3] = 1.0f;
 data[0].halfdims[0] = 1.0f; data[1].halfdims[0] = 1.0f;
 data[0].halfdims[1] = 1.0f; data[1].halfdims[1] = 1.0f;
 data[0].halfdims[2] = 1.0f; data[1].halfdims[2] = 1.0f;
 data[0].halfdims[3] = 1.0f; data[1].halfdims[3] = 1.0f;

 // create color buffer
 ErrorCode code = CreateImmutableFloat4ConstBuffer(color, &color_buffer);
 if(Fail(code)) {
    ErrorBox(TEXT("Failed to create color buffer."));
    return FALSE;
   }

 // create instance buffer
 code = CreateImmutableConstBuffer(&instance_buffer, n_aabb*sizeof(AABB_halfdim), &data[0]);
 if(Fail(code)) {
    ErrorBox(TEXT("Failed to create instance buffer."));
    return FALSE;
   }

 return TRUE;
}

void FreeAABBTest(void)
{
 if(color_buffer) color_buffer->Release();
 if(instance_buffer) instance_buffer->Release();
 color_buffer = nullptr;
 instance_buffer = nullptr;
}

void RenderAABBTest(real32 dt)
{
 // set per-model buffers and render
 SetVertexShaderPerModelBuffer(GetIdentityMatrix());
 if(color_buffer) SetPixelShaderPerModelBuffer(color_buffer);
 if(instance_buffer) RenderAABB(instance_buffer, n_aabb);
}