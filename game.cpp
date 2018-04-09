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
 std::ifstream ifile(filename);

 // insert filename into playlist
 maplist.push_back(filename);
 return EC_SUCCESS;
}

ErrorCode Game::RemoveMap(const STDSTRINGW& name)
{
 return EC_SUCCESS;
}

ErrorCode Game::LoadMap(const STDSTRINGW& name)
{
 return EC_SUCCESS;
}

void Game::FreeMap(void)
{
}

#pragma endregion MAP_FUNCTIONS
