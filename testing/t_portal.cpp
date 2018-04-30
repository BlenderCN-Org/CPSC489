#include "../stdafx.h"
#include "../stdgfx.h"
#include "../errors.h"
#include "../math.h"
#include "../vector3.h"
#include "../matrix4.h"
#include "../model.h"
#include "../camera.h"
#include "../xinput.h"
#include "../gfx.h"
#include "../viewport.h"
#include "tests.h"
#include "t_portal.h"

static MeshUTF model;
static std::unique_ptr<MeshUTFInstance> instance;

BOOL InitPortalTest(void)
{
 auto code = model.LoadModel(L"models\\cube.txt");
 if(Fail(code)) {
    DebugErrorCode(code, __LINE__, __FILE__);
    Error(code);
    return TRUE;
   }
 instance = std::make_unique<MeshUTFInstance>(model);
 instance->InitInstance();
 instance->SetAnimation(0xFFFFFFFFul);
 return TRUE;
}

void FreePortalTest(void)
{
 // release model
 instance.release();
 model.FreeModel();
}

void UpdatePortalTest(real32 dt)
{
 if(instance.get()) instance->Update(dt);
}

void RenderPortalTest(void)
{
 if(instance.get()) {
    ErrorCode code = instance->RenderModel();
    if(Fail(code)) {
       EndTest();
       return;
      }
   }
}