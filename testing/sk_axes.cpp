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

// controller variables
static uint32 controllerIndex = 0xFFFFFFFFul;

BOOL InitSkeletonAxesTest(void)
{
 //auto code = model.LoadModel(L"room.txt");
 auto code = model.LoadModel(L"untitled3.txt");
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

void FreeSkeletonAxesTest(void)
{
 instance->SetAnimation(0xFFFFFFFFul);
 instance->FreeInstance();
 model.FreeModel();
}

void RenderSkeletonAxesTest(real32 dt)
{
 // TODO: periodically check for controller instead
 if(controllerIndex == 0xFFFFFFFFul) {
    if(IsControllerAvailable()) controllerIndex = ReserveController();
   }

 // update controller
 if(controllerIndex != 0xFFFFFFFFul)
   {
    // lost the connection
    if(!IsControllerConnected(controllerIndex)) {
       ReleaseController(controllerIndex);
       controllerIndex = 0xFFFFFFFFul;
      }
    // connected and ready
    else
      {
       auto lpcs = GetControllerState(controllerIndex);
       if(lpcs)
         {
          // move at 5 m/s
          real32 distance = lpcs->JS_L_NORM*5.0f*dt;
          if(distance)
            {
             if(distance > 0.01f) distance = 0.01f;
             real32 v[3] = {
                lpcs->JS_L[1],
               -lpcs->JS_L[0],
               0.0f
             };
             GetOrbitCamera()->Move(v, distance);
             UpdateCamera();
            }
         }
      }
   }

 // render model instance
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