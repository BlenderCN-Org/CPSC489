// non-testing includes
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
#include "../collision.h"
#include "../map.h"

// testing includes
#include "tests.h"
#include "t_map.h"

// controller variables
static uint32 controllerIndex = 0xFFFFFFFFul;

BOOL InitMapTest(void)
{
 // get singleton map pointer
 Map* map = GetMap();
 ErrorCode code = map->LoadMap(L"models\\map_cube.txt");
 if(Fail(code)) return FALSE;

 return TRUE;
}

void FreeMapTest(void)
{
 // release controller
 ReleaseController(controllerIndex);
 controllerIndex = 0xFFFFFFFFul;

 // unload map
 GetMap()->FreeMap();
}

void RenderMapTest(real32 dt)
{
/*
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
*/
 // render map
 GetMap()->RenderMap(dt);
}