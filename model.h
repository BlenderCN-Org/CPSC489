#ifndef __CPSC489_MODEL_H
#define __CPSC489_MODEL_H

struct MeshASCIIJoint {
 STDSTRINGW name;
 uint32 parent;
 real32 position[3];
 real32 m[9];
};

struct MeshASCIIKeyFrame {
 uint32 frame;
 real32 position[3];
 real32 quaternion[4];
 real32 scale[3];
};

struct MeshASCIIBoneKeyFrame {
 STDSTRINGW bone;
 uint32 bone_index;
 std::vector<MeshASCIIKeyFrame> frames;
};

struct MeshASCIIAnimation {
 STDSTRINGW name;
 std::vector<MeshASCIIBoneKeyFrame>;
};

struct MeshASCIIMesh {
};

class MeshASCII {
 private :
  std::vector<MeshASCIIJoint> joints;
  std::vector<MeshASCIIAnimation> animations;
  std::vector<MeshASCIIMesh> meshes;
 public : 
  MeshASCII();
 ~MeshASCII();
};

#endif
