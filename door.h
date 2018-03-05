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
  
 public :
  DoorController();
  virtual ~DoorController();
};

#endif

