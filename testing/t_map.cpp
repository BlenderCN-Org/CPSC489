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
#include "../viewport.h"
#include "../game.h"

// testing includes
#include "tests.h"
#include "t_map.h"

static Game game;

BOOL InitMapTest(void)
{
 // initialize game
 auto code = game.InitGame();
 if(Fail(code, __LINE__, __FILE__)) return FALSE;

 // insert maps
 code = game.InsertMap(L"maps//room.txt");
 if(Fail(code, __LINE__, __FILE__)) return FALSE;

 // start game
 code = game.StartGame();
 if(Fail(code, __LINE__, __FILE__)) return FALSE;

 return TRUE;
}

void FreeMapTest(void)
{
 // stop and free game data
 game.StopGame();
 game.FreeGame();
}

void RenderMapTest(real32 dt)
{
}

/*
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
#include "../viewport.h"
#include "../game.h"

// testing includes
#include "tests.h"
#include "t_map.h"

// controller variables
static uint32 controllerIndex = 0xFFFFFFFFul;
static uint32 vpindex = 0xFFFFFFFFul;
static std::map<uint32, uint32> vpmap;

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
 // release viewport
 vpmap.erase(controllerIndex);
 vpindex = 0xFFFFFFFFul;

 // release controller
 ReleaseController(controllerIndex);
 controllerIndex = 0xFFFFFFFFul;

 // unload map
 GetMap()->FreeMap();
}

void RenderMapTest(real32 dt)
{
 // TODO: periodically check for controller instead
 if(controllerIndex == 0xFFFFFFFFul) {
    if(IsControllerAvailable())
      {
       // reserve controller
       controllerIndex = ReserveController();

       // find first viewport not associated controller
       for(uint32 i = 0; i < GetCanvasViewportNumber(); i++) {
           if(IsViewportEnabled(i) && (vpmap.find(controllerIndex) == vpmap.end())) {
              vpindex = i;
              vpmap.insert(std::make_pair(controllerIndex, i));
              break;
             }
          }
      }
   }

 // update controller
 if(controllerIndex != 0xFFFFFFFFul && vpindex != 0xFFFFFFFFul)
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
          real32 dL = lpcs->JS_L_NORM*2.0f*dt;
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

 // render map
 GetMap()->RenderMap(dt);
}
*/