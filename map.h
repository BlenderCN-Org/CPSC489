#ifndef __CS489_MAP_H
#define __CS489_MAP_H

class Map {
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
 // portal variables
 private :
  std::vector<std::vector<uint32>> cell_graph;
 public :
  ErrorCode LoadMap(LPCWSTR filename);
  void FreeMap(void);
  void RenderMap(void);
 public :
  Map();
  virtual ~Map();
 private :
  Map(const Map&) = delete;
  void operator =(const Map&) = delete;
};

Map* GetMap(void);

#endif
