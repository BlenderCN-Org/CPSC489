#ifndef __CS489_MAP_H
#define __CS489_MAP_H

#include "errors.h"
#include "matrix4.h"
#include "xaudio.h"
#include "model_v2.h"
#include "meshinst.h"

struct DoorController {
 OBB box;
 uint32 door_index;
 uint32 anim_start;
 uint32 anim_enter;
 uint32 anim_leave;
 uint32 sound_opening;
 uint32 sound_closing;
 bool   inside;
 real32 close_time;
};

struct CameraMarker {
 real32 location[3];
 matrix4D orientation;
 uint16 index;
 real32 speed;
 bool interpolate_speed;
 real32 fovy;
 bool interpolate_fovy;
};

struct CameraAnimation {
 STDSTRINGW name;
 real32 location[3];
 matrix4D orientation;
 uint16 start;
 uint32 n_markers;
 std::unique_ptr<CameraMarker[]> markers;
};

class Map {
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
  std::unique_ptr<MeshInstance[]> static_instances;
  std::unique_ptr<MeshInstance[]> moving_instances;
 // door controllers
 private :
  uint32 n_door_controllers;
  std::unique_ptr<DoorController[]> door_controllers;
 // camera animations
 private :
  uint32 n_cam_anims;
  std::unique_ptr<CameraAnimation[]> cam_anims;
 // portal variables
 private :
  std::vector<std::vector<uint32>> cell_graph;
 public :
  ErrorCode LoadMap(LPCWSTR filename);
  void FreeMap(void);
  void RenderMap(real32 dt);
 public :
  Map();
  virtual ~Map();
 private :
  Map(const Map&) = delete;
  void operator =(const Map&) = delete;
};

Map* GetMap(void);

#endif
