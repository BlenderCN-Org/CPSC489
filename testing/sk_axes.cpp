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
 auto code = model.LoadModel(L"untitled3.txt");
 if(Fail(code)) {
    DebugErrorCode(code, __LINE__, __FILE__);
    Error(code);
    return TRUE;
   }
 instance = std::make_unique<MeshUTFInstance>(model);
 instance->InitInstance();
 instance->SetAnimation(0); // 0xFFFFFFFFul);
 return TRUE;
}

void FreeSkeletonAxesTest(void)
{
 // release controller
 ReleaseController(controllerIndex);
 controllerIndex = 0xFFFFFFFFul;

 // release model
 if(instance.get()) instance.release();
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
          real32 dL = lpcs->JS_L_NORM*5.0f*dt;
          real32 dR = lpcs->JS_R_NORM*90.0f*dt;
          if(dL)
            {
             //if(dL > 0.01f) dL = 0.01f;
             real32 vL[3] = {
                lpcs->JS_L[1],
               -lpcs->JS_L[0],
               0.0f
             };
             GetOrbitCamera()->Move(vL, dL);
            }
          if(dR) {
             real32 vR[2] = {
               -lpcs->JS_R[0], // Z-axis rotation
               -lpcs->JS_R[1], // Y-axis rotation
             };
             GetOrbitCamera()->ThumbstickOrbit(vR, dR);
            }
          UpdateCamera();
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