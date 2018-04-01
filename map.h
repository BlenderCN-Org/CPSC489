#ifndef __CS489_MAP_H
#define __CS489_MAP_H

#include "xaudio.h"

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

class Map {
 // music and sounds
 private :
  uint32 n_sounds;
  std::unique_ptr<STDSTRINGW[]> soundlist;
  std::unique_ptr<SoundData*[]> sounds;
 // models
 private :
  uint32 n_static;
  uint32 n_moving;
  uint32 n_static_instances;
  uint32 n_moving_instances;
  std::unique_ptr<std::shared_ptr<MeshUTF>[]> static_models;
  std::unique_ptr<std::shared_ptr<MeshUTF>[]> moving_models;
  std::unique_ptr<std::shared_ptr<MeshUTFInstance>[]> static_instances;
  std::unique_ptr<std::shared_ptr<MeshUTFInstance>[]> moving_instances;
 // door controllers
 private :
  uint32 n_door_controllers;
  std::unique_ptr<DoorController[]> door_controllers;
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
