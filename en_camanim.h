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
#ifndef __CS489_ENTITY_CAMANIM_H
#define __CS489_ENTITY_CAMANIM_H

#include "errors.h"
#include "entity.h"
#include "en_cammark.h"
#include "vector3.h"
#include "matrix4.h"

// Camera Marker List
class CameraMarkerList : public Entity {

 // Variables
 private :
  uint32 player;
  uint32 start;
  uint32 n_markers;
  std::unique_ptr<CameraMarker[]> markers;
 private :
  uint32 curr; // marker index
  uint32 next; // marker index
  real32 base; // time
  real32 time; // time
  vector3D P;  // position
  vector3D E;  // euler angle
  matrix4D C;  // camera matrix

 // Member Functions
 public :
  void SetPlayerFocus(uint32 n);
  uint32 GetPlayerFocus(void)const;
  void SetMarkers(uint32 n, std::unique_ptr<CameraMarker[]>& data);
  const CameraMarker* GetMarkers(void)const;
  ErrorCode SetStartMarker(uint32 index);
  uint32 GetStartMarker(void)const;
  void Update(real32 dt);
 // Operators
 public :
  CameraMarker& operator [](size_t index);
  const CameraMarker& operator [](size_t index)const;

 // Events
 public :
  virtual void EvTrigger(Entity* source) {}

 // Special Member Functions
 public :
  CameraMarkerList();
  CameraMarkerList(const CameraMarkerList&) = delete;
  virtual ~CameraMarkerList();
  void operator =(const CameraMarkerList&) = delete;
};

#endif
