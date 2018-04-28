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

#ifndef __CS489_MAP_H
#define __CS489_MAP_H

#include "errors.h"
#include "vector3.h"
#include "matrix4.h"
#include "collision.h" // for OBB, move this later
#include "xaudio.h"
#include "model_v2.h"
#include "meshinst.h"

// Entity Headers
#include "en_camanim.h"
#include "en_doorctrl.h"

struct EntityMarker {
 STDSTRINGW name;
 real32 location[3];
 matrix4D orientation;
 real32 euler[3];
 uint32 index;
 real32 speed;
 bool interpolate_speed;
 uint32 anim;
 bool anim_loop;
 uint32 sound;
 bool sound_loop;
};

struct EntityMarkerList {
 STDSTRINGW name;
 real32 location[3];
 matrix4D orientation;
 uint32 instance;
 uint32 n_markers;
 std::unique_ptr<EntityMarker[]> markers;
};

struct Portal {
 STDSTRINGW name;
 uint32 n;
 std::unique_ptr<vector3D[]> vb;
 std::unique_ptr<uint16[]> ib;
};

struct PortalConnection {
 uint32 instance; // connected to this model instance
 uint32 mesh;     // mesh index
 uint32 portal;   // via this portal
};

struct PortalCell {
 uint32 instance;
 uint32 mesh;
 uint32 n_conn;
 std::unique_ptr<PortalConnection[]> conn; // connections
};

struct DoorControllerData {
 uint32 size;
 std::unique_ptr<DoorController[]> data;
};

struct CameraMarkerData {
 uint32 size;
 std::unique_ptr<CameraMarkerList[]> data;
};

struct EntityMarkerData {
 uint32 size;
 std::unique_ptr<EntityMarkerList[]> data;
};

struct PortalData {
 uint32 size;
 std::unique_ptr<Portal[]> data;
};

struct PortalCellData {
 uint32 size;
 std::unique_ptr<PortalCell[]> data;
};

class Map {
 // map variables
 private :
  STDSTRINGW name;
 // music and sounds
 private :
  uint32 n_sounds;
  std::unique_ptr<STDSTRINGW[]> soundlist;
  std::unique_ptr<SoundData*[]> sounds;
  uint32 sound_start;
 // models
 private :
  uint32 n_static;
  uint32 n_moving;
  uint32 n_static_instances;
  uint32 n_moving_instances;
  std::unique_ptr<MeshData[]> static_models;
  std::unique_ptr<MeshData[]> moving_models;
  std::map<STDSTRINGW, uint32> static_instance_map;
  std::map<STDSTRINGW, uint32> moving_instance_map;
  std::unique_ptr<MeshInstance[]> static_instances;
  std::unique_ptr<MeshInstance[]> moving_instances;
 // file data
 private :
  DoorControllerData dcd;
  CameraMarkerData cmd;
  EntityMarkerData emd;
 // portal variables
 private :
  PortalData portals;
  PortalCellData cells;
 // Private Loading Functions
 private :
  ErrorCode LoadStaticModels(std::deque<std::string>& linelist);
  ErrorCode LoadDynamicModels(std::deque<std::string>& linelist);
  ErrorCode LoadSounds(std::deque<std::string>& linelist);
  ErrorCode LoadStaticInstances(std::deque<std::string>& linelist);
  ErrorCode LoadDynamicInstances(std::deque<std::string>& linelist);
  ErrorCode LoadCameraMarkerLists(std::deque<std::string>& linelist);
  ErrorCode LoadEntityMarkerLists(std::deque<std::string>& linelist);
  ErrorCode LoadDoorControllers(std::deque<std::string>& linelist);
  ErrorCode LoadPortals(std::deque<std::string>& linelist);
  ErrorCode LoadCells(std::deque<std::string>& linelist);
 // Private Unloading Functions
 private :
  void FreeStaticModels(void);
  void FreeDynamicModels(void);
  void FreeSounds(void);
  void FreeStaticInstances(void);
  void FreeDynamicInstances(void);
  void FreeCameraMarkerLists(void);
  void FreeEntityMarkerLists(void);
  void FreeDoorControllers(void);
  void FreePortals(void);
  void FreeCells(void);
 public :
  ErrorCode LoadMap(LPCWSTR filename);
  void FreeMap(void);
  void RenderMap(real32 dt);
 public :
  MeshInstance* GetStaticMeshInstance(const STDSTRINGW& name);
  MeshInstance* GetStaticMeshInstance(uint32 index);
  MeshInstance* GetDynamicMeshInstance(const STDSTRINGW& name);
  MeshInstance* GetDynamicMeshInstance(uint32 index);
  SoundData* GetSoundData(uint32 index);
 public :
  Map();
  virtual ~Map();
 private :
  Map(const Map&) = delete;
  void operator =(const Map&) = delete;
};

#endif
