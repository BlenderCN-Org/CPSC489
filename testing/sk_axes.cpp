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
 auto code = model.LoadModel(L"bl_TestVertexGroups.txt");
 if(Fail(code)) {
    Error(code, __LINE__, __FILE__);
    return TRUE;
   }
 instance = std::make_unique<MeshUTFInstance>(model);
 return TRUE;
}

void FreeSkeletonAxesTest(void)
{
 instance.release();
 model.FreeModel();
}

void RenderSkeletonAxesTest(void)
{
 model.RenderModel();
 if(instance.get()) {
    ErrorCode code = instance->RenderModel();
    if(Fail(code)) {
       EndTest();
       return;
      }
   }
}