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
#ifndef __CS489_ENTITY_CAMMARK_H
#define __CS489_ENTITY_CAMMARK_H

#include "entity.h"

// Camera Marker
class CameraMarker : public Entity {

 // Variables
 private :
  real32 euler[3];
  real32 delta;
  bool interpolate_time;
  real32 fovy;
  bool interpolate_fovy;

 // Member Functions
 public :
  void SetEulerAngle(const real32* angle);
  const real32* GetEulerAngle(void)const;
  void SetTime(real32 value);
  real32 GetTime(void)const;
  void SetInterpolateTimeFlag(bool flag);
  bool GetInterpolateTimeFlag(void)const;
  void SetFOVY(real32 value);
  real32 GetFOVY(void)const;
  void SetInterpolateFOVYFlag(bool flag);
  bool GetInterpolateFOVYFlag(void)const;

 // Events
 public :
  virtual void EvTrigger(Entity* source) {}

 // Special Member Functions
 public :
  CameraMarker();
  CameraMarker(const CameraMarker&) = delete;
  virtual ~CameraMarker();
  void operator =(const CameraMarker&) = delete;
};

#endif
