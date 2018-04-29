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
#include "tests.h"
#include "sk_axes.h"

static MeshUTF model;
static std::unique_ptr<MeshUTFInstance> instance;

BOOL InitSkeletonAxesTest(void)
{
 auto code = model.LoadModel(L"models\\door.txt");
 if(Fail(code)) {
    DebugErrorCode(code, __LINE__, __FILE__);
    Error(code);
    return TRUE;
   }
 instance = std::make_unique<MeshUTFInstance>(model);
 instance->InitInstance();
 instance->SetAnimation(1); // 0xFFFFFFFFul);
 return TRUE;
}

void FreeSkeletonAxesTest(void)
{
 // release model
 if(instance.get()) instance.release();
 model.FreeModel();
}

void UpdateSkeletonAxesTest(real32 dt)
{
 // update model instance
 if(instance.get())
    instance->Update(dt);
}

void RenderSkeletonAxesTest(void)
{
 // render model instance
 if(instance.get()) {
    ErrorCode code = instance->RenderModel();
    if(Fail(code)) {
       EndTest();
       return;
      }
   }
}

