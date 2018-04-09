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
#include "stdafx.h"
#include "game.h"

static LPCWSTR filename = L"map.txt"; // L"levels//map1.txt"

#pragma region SPECIAL_MEMBER_FUNCTIONS

Game::Game()
{
 n_players = 0;
 players.reset(new PlayerEntity[MAX_PLAYERS]);

 delta = 0.0f;
}

Game::Game(Game&& other)
{
}

Game& Game::operator =(Game&& other)
{
} 

Game::~Game()
{
}

#pragma endregion SPECIAL_MEMBER_FUNCTIONS

#pragma region GAME_FUNCTIONS

ErrorCode Game::InitGame(void)
{
 return EC_SUCCESS;
}

void Game::FreeGame(void)
{
}

void Game::UpdateGame(real32 dt)
{
}

#pragma endregion GAME_FUNCTIONS

#pragma region CONTROLLER_FUNCTIONS

void Game::PollForControllers(void)
{
} 

#pragma endregion CONTROLLER_FUNCTIONS

#pragma region MAP_FUNCTIONS

ErrorCode Game::InsertMap(const STDSTRINGW& filename)
{
 // make sure file exists
 std::ifstream ifile(filename);
 if(!ifile) return DebugErrorCode(EC_FILE_OPEN, __LINE__, __FILE__);

 // read first line to get map name
 char buffer[1024];
 ifile.getline(buffer, 1024);
 if(ifile.fail()) return DebugErrorCode(EC_FILE_READ, __LINE__, __FILE__);
 if(!strlen(buffer)) return DebugErrorCode(EC_MAP_NAME, __LINE__, __FILE__);

 // convert map name to UTF16
 STDSTRINGW mapname = ConvertUTF8ToUTF16(buffer);
 if(!mapname.length()) return DebugErrorCode(EC_MAP_NAME, __LINE__, __FILE__);

 // insert filename into playlist
 maplist.push_back(std::make_pair(filename, mapname));
 return EC_SUCCESS;
}

void Game::RemoveMap(const STDSTRINGW& name, bool all)
{
 // the same map can be added multiple times
 std::deque<MAPITEM>::iterator iter = maplist.begin();
 while(iter != maplist.end()) {
       if(iter->second == name) {
          iter = maplist.erase(iter);
          if(!all) break;
         }
       else iter++;
      }
}

ErrorCode Game::LoadMap(const STDSTRINGW& name)
{
 return EC_SUCCESS;
}

void Game::FreeMap(void)
{
}

#pragma endregion MAP_FUNCTIONS
