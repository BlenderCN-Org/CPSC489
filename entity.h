/*
** CPSC489 Game Development Project
** Copyright (c) 2018  Steven Emory <mobysteve@gmail.com>
**
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef __CS489_ENTITY_H
#define __CS489_ENTITY_H

#include "vector3.h"
#include "matrix4.h"
#include "model_v2.h"
#include "meshinst.h"

class Entity {

 // Base Properties
 private :
  static uint32 baseID;
  static std::map<uint32, Entity*> entity_map;
  STDSTRINGW name;
  uint32 id;
  Entity* targets[16];

 // Positioning Properties
 private :
  vector3D position;
  matrix4D orientation;

 // Model Properties
 private :
  MeshInstance* mesh;

 // Static Member Functions
 public :
  static Entity* GetEntity(uint32 eid);

 //
 public :
  const STDSTRINGW& GetName(void)const { return name; }
  uint32 GetID(void)const { return id; }

 //
 public :
  virtual void Render(void);

 // Events
 public :
  virtual void EvTrigger(Entity* source) = 0;

 // Special Member Functions
 public :
  Entity();
  Entity(const Entity&) = delete;
  virtual ~Entity();
  void operator =(const Entity&) = delete;
};

#endif
