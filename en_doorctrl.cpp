#include "stdafx.h"
#include "en_doorctrl.h"

#include "map.h"
#include "xaudio.h"

DoorController::DoorController()
{
 door_index = 0xFFFFFFFFul;
 anims[0] = anims[1] = anims[2] = 0xFFFFFFFFul;
 sound[0] = sound[1] = sound[2] = 0xFFFFFFFFul;
 close_time = 5.0f;
 delta = 0.0f;
 inside = false;
}

DoorController::~DoorController()
{
}

void DoorController::SetOBB(const real32* halfdims)
{
 // use entity location
 box.center[0] = location[0];
 box.center[1] = location[1];
 box.center[2] = location[2];

 // use entity orientation
 box.x[0] = orientation[0x0]; box.x[1] = orientation[0x1]; box.x[2] = orientation[0x2];
 box.y[0] = orientation[0x4]; box.y[1] = orientation[0x5]; box.y[2] = orientation[0x6];
 box.z[0] = orientation[0x8]; box.z[1] = orientation[0x9]; box.z[2] = orientation[0xA];

 // set half dimensions
 box.widths[0] = halfdims[0];
 box.widths[1] = halfdims[1];
 box.widths[2] = halfdims[2];
}

void DoorController::SetOBB(const real32* P, const real32* M, const real32* halfdims)
{
 // use custom location
 box.center[0] = P[0];
 box.center[1] = P[1];
 box.center[2] = P[2];

 // use custom orientation
 box.x[0] = M[0x0]; box.x[1] = M[0x1]; box.x[2] = M[0x2];
 box.y[0] = M[0x4]; box.y[1] = M[0x5]; box.y[2] = M[0x6];
 box.z[0] = M[0x8]; box.z[1] = M[0x9]; box.z[2] = M[0xA];

 // set half dimensions
 box.widths[0] = halfdims[0];
 box.widths[1] = halfdims[1];
 box.widths[2] = halfdims[2];
}

const OBB& DoorController::GetOBB(void)const
{
 return box;
}

void DoorController::SetDoorIndex(uint32 index)
{
 door_index = index;
}

uint32 DoorController::GetDoorIndex(void)const
{
 return door_index;
}

void DoorController::SetAnimations(uint32 idle, uint32 open, uint32 close)
{
 anims[0] = idle;
 anims[1] = open;
 anims[2] = close;
}

const uint32* DoorController::GetAnimations(void)const
{
 return &anims[0];
}

void DoorController::SetSounds(uint32 idle, uint32 open, uint32 close)
{
 sound[0] = idle;
 sound[1] = open;
 sound[2] = close;
}

const uint32* DoorController::GetSounds(void)const
{
 return &sound[0];
}

void DoorController::SetClosingTime(real32 dt)
{
 if(!(dt < 0.0f)) close_time = dt;
}

real32 DoorController::GetClosingTime(void)const
{
 return close_time;
}
#include "viewport.h"
void DoorController::Poll(real32 dt)
{
 // nothing to do, door is disabled
 if(!GetActiveFlag()) return;

 // are any viewport cameras inside?
 bool intersect = false;
 for(uint32 i = 0; i < GetCanvasViewportNumber(); i++) {
     if(IsViewportEnabled(i)) {
        auto camera = GetViewportCamera(i);
        real32 orbit[3];
        camera->GetOrbitPoint(orbit);
        if(OBB_intersect(this->box, orbit)) {
           intersect = true;
           break;
          }
       }
    }

 // TODO:
 // If there is no closing animation, assume that the door can't be closed once
 // it is open. Once open, we can deactivate the door to stop future OBB inter-
 // section testing.

 // now inside
 if(intersect)
   {
    // was outside
    if(!inside) {
       auto instance = GetMap()->GetDynamicMeshInstance(door_index);
       instance->SetAnimation(anims[1], false);
       if(!inside) {
          if(sound[1] != 0xFFFFFFFFul) PlayVoice(GetMap()->GetSoundData(sound[1]), false);
          inside = true;
         }
      }
   }
 // now outside
 else
   {
    // was inside
    if(inside) {
       delta = close_time;
       inside = false;
      }
    // was outside
    else if(delta) {
       delta -= dt;
       if(!(delta > 0.0f)) {
          auto instance = GetMap()->GetDynamicMeshInstance(door_index);
          instance->SetAnimation(anims[2], false);
          if(sound[2] != 0xFFFFFFFFul) PlayVoice(GetMap()->GetSoundData(sound[2]), false);
          delta = 0.0f;
         }
      }
   }
}

void DoorController::EvTrigger(Entity* source)
{
}