#include "../stdafx.h"
#include "../stdgfx.h"
#include "../errors.h"
#include "../math.h"
#include "../vector3.h"
#include "../matrix4.h"
#include "../model_v2.h"
#include "../meshinst.h"
#include "../camera.h"
#include "../xinput.h"
#include "../gfx.h"
#include "../viewport.h"
#include "tests.h"
#include "flyby.h"

static MeshData model;
static std::unique_ptr<MeshInstance> instance;
static uint32 vpindex = 0xFFFFFFFFul;
static std::map<uint32, uint32> vpmap;

// controller variables
static uint32 controllerIndex = 0xFFFFFFFFul;

BOOL InitFlybyTest(void)
{
 auto code = model.LoadMeshUTF(L"models\\map.txt");
 if(Fail(code)) {
    DebugErrorCode(code, __LINE__, __FILE__);
    Error(code);
    return TRUE;
   }
 instance = std::make_unique<MeshInstance>();
 instance->InitInstance(&model);
 instance->SetAnimation(0xFFFFFFFFul);
 return TRUE;
}

void FreeFlybyTest(void)
{
 // release viewport
 vpmap.erase(controllerIndex);
 vpindex = 0xFFFFFFFFul;

 // release controller
 ReleaseController(controllerIndex);
 controllerIndex = 0xFFFFFFFFul;

 // release model
 instance.release();
 model.Free();
}

void UpdateFlybyTest(real32 dt)
{
 // TODO: periodically check for controller instead
 if(controllerIndex == 0xFFFFFFFFul) {
    if(IsControllerAvailable())
      {
       // reserve controller
       controllerIndex = ReserveController();

       // find first viewport not associated controller
       for(uint32 i = 0; i < 4; i++) {
           if(IsViewportEnabled(i) && vpmap.find(controllerIndex) == vpmap.end()) {
              vpindex = i;
              vpmap.insert(std::make_pair(controllerIndex, i));
              break;
             }
          }
      }
   }

 // update controller
 if(controllerIndex != 0xFFFFFFFFul)
   {
    // lost the connection
    if(!IsControllerConnected(controllerIndex)) {
       vpmap.erase(controllerIndex);
       vpindex = 0xFFFFFFFFul;
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
             GetViewportCamera(vpindex)->Move(vL, dL);
            }
          if(dR) {
             real32 vR[2] = {
               -lpcs->JS_R[0], // Z-axis rotation
               -lpcs->JS_R[1], // Y-axis rotation
             };
             GetViewportCamera(vpindex)->ThumbstickOrbit(vR, dR);
            }
          UpdateViewportCamera(vpindex);
         }
      }
   }

 // update model instance
 if(instance.get())
    instance->Update(dt);
}

void RenderFlybyTest(void)
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