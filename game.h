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
#ifndef __CS489_GAME_H
#define __CS489_GAME_H

#include "errors.h"
#include "player.h"
#include "map.h"

class Timer {
 private :
  real32 delta;
  real32 limit;
  bool started;
 public :
  bool triggered(void)const {
   return (started && delta == limit);
  }
  void start(void) {
   started = true;
  }
  void stop(void) {
   started = false;
  }
  void update(real32 dt) {
   if(started) {
      delta += dt;
      if(limit < delta) delta = limit;
     }
  }
  void reset(real32 time) {
   started = false;
   delta = 0.0f;
   limit = time;
  }
  void reset(void) {
   started = false;
   delta = 0.0f;
  }
 public :
  Timer() {
   started = false;
   delta = 0.0f;
   limit = 1.0f;
  }
  Timer(real32 value) {
   started = false;
   delta = 0.0f;
   limit = value;
  }
};

class Game {

 // Player Variables
 private :
  static const uint32 MAX_PLAYERS = 4;
  uint32 n_players;
  std::unique_ptr<PlayerEntity[]> players;

 // Map Variables
 private :
  typedef std::pair<STDSTRINGW, STDSTRINGW> MAPITEM;
  std::deque<MAPITEM> maplist;
  size_t map_index;
  Map map;

 // Time Variables
 private :
  real32 delta;

 // Controller Variables
 private :
  uint32 controller[4];
  Timer ctrlpoller;

 // Game Functions
 public :
  ErrorCode InitGame(void);
  void FreeGame(void);
  ErrorCode StartGame(void);
  void StopGame(void);
  void PauseGame(void);
  void UpdateGame(real32 dt);

 // Controller Functions
 public :
  void PollForControllers(real32 dt); 

  // Map Functions
 public :
  ErrorCode InsertMap(const STDSTRINGW& filename);
  void RemoveMap(const STDSTRINGW& name, bool all = true);
  ErrorCode LoadMap(const STDSTRINGW& name);
  void FreeMap(void);

 // Player Functions
 public :
  uint32 GetNumberOfPlayers(void);
  void SetNumberOfPlayers(uint32 n);
  void AppendPlayer(void);
  void RemovePlayer(uint32 index);

 // Special Member Functions
 public :
  Game();
  Game(const Game& other) = delete;
  void operator =(const Game& other) = delete;
  Game(Game&& other);
  Game& operator =(Game&& other); 
  virtual ~Game();
};

#endif
