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
#ifndef __CS489_PLAYER_H
#define __CS489_PLAYER_H

#include "entity.h"

class PlayerEntity : public Entity {

 // Events
 public :
  virtual void EvTrigger(Entity* source);

 // Special Member Functions
 public :
  PlayerEntity();
  PlayerEntity(const PlayerEntity&) = delete;
  virtual ~PlayerEntity();
  void operator =(const PlayerEntity&) = delete;
};

#endif
