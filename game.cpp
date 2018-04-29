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

#include "viewport.h"
#include "xinput.h"

static Game game;
Game* GetGame(void) { return &game; }

#pragma region SPECIAL_MEMBER_FUNCTIONS

Game::Game()
{
 // set player variables
 n_players = 0;
 p1cam = nullptr;
 p2cam = nullptr;
 p3cam = nullptr;
 p4cam = nullptr;

 // set map variables
 map_index = 0xFFFFFFFFul;

 // set time variables
 delta = 0.0f;

 // set controller variables
 for(int i = 0; i < 4; i++) controller[i] = 0xFFFFFFFFul;
 ctrlpoller.reset(5.0f);
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
 // set player variables
 n_players = 3;
 players.reset(new PlayerEntity[MAX_PLAYERS]);

 // set and enable viewport(s)
 auto code = SetCanvasViewportNumber(n_players);
 if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);
 for(uint32 i = 0; i < n_players; i++) EnableViewport(i, true);
 UpdateCanvas();

 // set map variables
 maplist.clear();
 map_index = 0xFFFFFFFFul;

 // set time variables
 delta = 0.0f;

 // set controller variables
 ctrlpoller.reset(5.0f);
 PollForControllers(0);

 return EC_SUCCESS;
}

void Game::FreeGame(void)
{
 // clear controller variables
 ReleaseControllers();
 ctrlpoller.stop();

 // clear time variables
 delta = 0.0f;

 // clear map variables
 map.FreeMap();
 maplist.clear();
 map_index = 0xFFFFFFFFul;

 // clear player variables
 n_players = 0;
 players.reset();
}

ErrorCode Game::StartGame(void)
{
 // load first map
 if(maplist.empty()) return DebugErrorCode(EC_UNKNOWN, __LINE__, __FILE__);
 map_index = 0;
 auto code = map.LoadMap(maplist[map_index].first.c_str());
 if(Fail(code)) return DebugErrorCode(code, __LINE__, __FILE__);

 // look through camera animation lists for rail cameras
 for(uint32 i = 0; i < map.cmd.size; i++)
    {
     // all players use same camera viewpoint
     CameraMarkerList& cml = map.cmd.data[i];
     if(cml.GetPlayerFocus() == 0xFFFFFFFFul) {
        p1cam = &cml;
        p2cam = &cml;
        p3cam = &cml;
        p4cam = &cml;
        break;
       }
     // player 1 unique camera
     else if(!p1cam && cml.GetPlayerFocus() == 0)
        p1cam = &cml;
     // player 2 unique camera
     else if(!p2cam && cml.GetPlayerFocus() == 1)
        p2cam = &cml;
     // player 3 unique camera
     else if(!p3cam && cml.GetPlayerFocus() == 2)
        p3cam = &cml;
     // player 4 unique camera
     else if(!p4cam && cml.GetPlayerFocus() == 3)
        p4cam = &cml;
    }

 return EC_SUCCESS;
}

void Game::StopGame(void)
{
 // unload current map
 if(map_index != 0xFFFFFFFFul) map.FreeMap();
 map_index = 0xFFFFFFFFul;
}

void Game::PauseGame(void)
{
}

void Game::UpdateGame(real32 dt)
{
 // update time
 real32 prev_delta = delta;
 real32 next_delta = delta + dt;

 // poll for controllers every five seconds
 PollForControllers(dt);
 UpdateControllers(dt);

 // update rail cameras
 if(p1cam) {
    p1cam->Update(dt);
    OrbitCamera* camera = GetViewportCamera(0);
    camera->RepositionEulerXYZ(p1cam->GetCameraPosition(), p1cam->GetCameraEulerXYZ());
    UpdateViewportCamera(0);
   }
 if(p2cam) {
    p2cam->Update(dt);
    OrbitCamera* camera = GetViewportCamera(1);
    camera->RepositionEulerXYZ(p2cam->GetCameraPosition(), p2cam->GetCameraEulerXYZ());
    UpdateViewportCamera(1);
   }
 if(p3cam) {
    p3cam->Update(dt);
    OrbitCamera* camera = GetViewportCamera(2);
    camera->RepositionEulerXYZ(p3cam->GetCameraPosition(), p3cam->GetCameraEulerXYZ());
    UpdateViewportCamera(2);
   }
 if(p4cam) {
    p4cam->Update(dt);
    //OrbitCamera* camera = GetViewportCamera(3);
    //camera->RepositionEulerXYZ(p4cam->GetCameraPosition(), p4cam->GetCameraEulerXYZ());
    //UpdateViewportCamera(3);
   }

 // render map
 map.RenderMap(dt);
}

#pragma endregion GAME_FUNCTIONS

#pragma region CONTROLLER_FUNCTIONS

void Game::PollForControllers(real32 dt)
{
 // update polling timer
 ctrlpoller.update(dt);
 if(!ctrlpoller.triggered()) return;

 // check for disconnected controllers and release them
 for(uint32 i = 0; i < n_players; i++) {
     if(controller[i] != 0xFFFFFFFFul && !IsControllerConnected(controller[i])) {
        ReleaseController(controller[i]);
        controller[i] = 0xFFFFFFFFul;
       }
    }

 // reserve controllers for each player (if possible)
 for(uint32 i = 0; i < n_players; i++) {
     if(controller[i] == 0xFFFFFFFFul && IsControllerAvailable())
        controller[i] = ReserveController();
    }

 // reset five second timer
 ctrlpoller.reset();
}

void Game::ReleaseControllers(void)
{
 for(uint32 i = 0; i < 4; i++) {
     if(controller[i] != 0xFFFFFFFFul && !IsControllerConnected(controller[i])) {
        ReleaseController(controller[i]);
        controller[i] = 0xFFFFFFFFul;
       }
    }
 ctrlpoller.reset();
}

void Game::UpdateControllers(real32 dt)
{
 // for each player
 for(uint32 i = 0; i < n_players; i++)
    {
     // no controller
     if(controller[i] == 0xFFFFFFFFul) continue;

     // update
     if(IsControllerConnected(controller[i]))
       {
        auto lpcs = GetControllerState(controller[i]);
        if(lpcs)
          {
           // move at 5 m/s
           real32 dL = lpcs->JS_L_NORM*2.0f*dt;
           real32 dR = lpcs->JS_R_NORM*90.0f*dt;
           if(dL)
             {
              //if(dL > 0.01f) dL = 0.01f;
              real32 vL[3] = {
                 lpcs->JS_L[1],
                -lpcs->JS_L[0],
                0.0f
              };
              GetViewportCamera(i)->Move(vL, dL);
             }
           if(dR) {
              real32 vR[2] = {
                -lpcs->JS_R[0], // Z-axis rotation
                -lpcs->JS_R[1], // Y-axis rotation
              };
              GetViewportCamera(i)->ThumbstickOrbit(vR, dR);
             }
           UpdateViewportCamera(i);
          }
       }
     // disconnected
     else {
        ReleaseController(controller[i]);
        controller[i] = 0xFFFFFFFFul;
       }
    }
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

#pragma region PLAYER_FUNCTIONS

void Game::SetNumberOfPlayers(uint32 n)
{
}

void Game::AppendPlayer(void)
{
}

void Game::RemovePlayer(uint32 index)
{
}

#pragma endregion PLAYER_FUNCTIONS
