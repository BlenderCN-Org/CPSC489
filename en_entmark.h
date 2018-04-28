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
#ifndef __CS489_ENTITY_ENTMARK_H
#define __CS489_ENTITY_ENTMARK_H

#include "errors.h"
#include "entity.h"

// Entity Marker
class EntityMarker : public Entity {

 // Variables
 private :
  real32 euler[3];
  real32 speed;
  bool interpolate_speed;
  uint32 anim;
  bool anim_loop;
  uint32 sound;
  bool sound_loop;

 // Member Functions
 public :
  void SetEulerAngle(const real32* angle);
  const real32* GetEulerAngle(void)const;
  void SetSpeed(real32 value);
  real32 GetSpeed(void)const;
  void SetInterpolateSpeedFlag(bool flag);
  bool GetInterpolateSpeedFlag(void)const;
  ErrorCode SetAnimation(uint32 index);
  uint32 GetAnimation(void)const;
  void SetAnimationLoopFlag(bool flag);
  bool GetAnimationLoopFlag(void)const;
  ErrorCode SetSound(uint32 index);
  uint32 GetSound(void)const;
  void SetSoundLoopFlag(bool flag);
  bool GetSoundLoopFlag(void)const;

 // Events
 public :
  virtual void EvTrigger(Entity* source) {}

 // Special Member Functions
 public :
  EntityMarker();
  EntityMarker(const EntityMarker&) = delete;
  virtual ~EntityMarker();
  void operator =(const EntityMarker&) = delete;
};

#endif
