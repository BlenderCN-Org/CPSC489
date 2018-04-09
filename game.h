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
  Map map;

 // Time Variables
 private :
  real32 delta;

 // Game Functions
 public :
  ErrorCode InitGame(void);
  void FreeGame(void);
  void UpdateGame(real32 dt);

 // Controller Functions
 public :
  void PollForControllers(void); 

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
