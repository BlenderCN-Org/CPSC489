#ifndef __CS_DOOR_H
#define __CS_DOOR_H

class Entity {};

enum class DoorType {
 AUTOMATIC,
 
};

// Door Controller
class DoorController : public Entity {

 private :
  float dim[3]; // dimensions <dx, dy, dz>
  
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

