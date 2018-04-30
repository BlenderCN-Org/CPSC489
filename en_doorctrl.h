#ifndef __CS_DOOR_H
#define __CS_DOOR_H

#include "collision.h" // for OBB, move this later
#include "entity.h"

// Door Controller
class DoorController : public Entity {

 // Variables
 private :
  OBB box;
  uint32 door_index;
  uint32 anims[3];
  uint32 sound[3];
  real32 close_time;
  real32 delta;
  bool   inside;
  
 public :
  void SetOBB(const real32* halfdims);
  void SetOBB(const real32* P, const real32* M, const real32* halfdims);
  const OBB& GetOBB(void)const;
  void SetDoorIndex(uint32 index);
  uint32 GetDoorIndex(void)const;
  void SetAnimations(uint32 idle, uint32 open, uint32 close);
  const uint32* GetAnimations(void)const;
  void SetSounds(uint32 idle, uint32 open, uint32 close);
  const uint32* GetSounds(void)const;
  void SetClosingTime(real32 dt);
  real32 GetClosingTime(void)const;
  void Poll(real32 dt);

 // Events
 public :
  virtual void EvTrigger(Entity* source);

 // Special Member Functions
 public :
  DoorController();
  DoorController(const DoorController&) = delete;
  virtual ~DoorController();
  void operator =(const DoorController&) = delete;
};

#endif
