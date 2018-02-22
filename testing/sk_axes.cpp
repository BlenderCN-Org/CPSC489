#include "../stdafx.h"
#include "../stdgfx.h"
#include "../errors.h"
#include "../math.h"
#include "../vector3.h"
#include "../matrix4.h"
#include "../model.h"
#include "../gfx.h"
#include "tests.h"
#include "sk_axes.h"

static MeshUTF model;
static std::unique_ptr<MeshUTFInstance> instance;

BOOL InitSkeletonAxesTest(void)
{
 auto code = model.LoadModel(L"room.txt");
 //auto code = model.LoadModel(L"untitled3.txt");
 if(Fail(code)) {
    DebugErrorCode(code, __LINE__, __FILE__);
    Error(code);
    return TRUE;
   }
 instance = std::make_unique<MeshUTFInstance>(model);
 instance->InitInstance();
 //instance->SetAnimation(0);
 return TRUE;
}

void FreeSkeletonAxesTest(void)
{
 instance->SetAnimation(0xFFFFFFFFul);
 instance->FreeInstance();
 model.FreeModel();
}

void RenderSkeletonAxesTest(real32 dt)
{
 model.RenderModel();
 if(instance.get()) {
    instance->Update(dt);
    ErrorCode code = instance->RenderModel();
    if(Fail(code)) {
       EndTest();
       return;
      }
   }
}