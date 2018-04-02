#ifndef __CS489_ENTITY_H
#define __CS489_ENTITY_H

#include "vector3.h"

class Entity {
 private :
 public :
  Entity();
  virtual ~Entity();
 public :
  Entity(const Entity&) = delete;
  void operator =(const Entity&) = delete;
};

#endif
