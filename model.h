#ifndef __CPSC489_MODEL_H
#define __CPSC489_MODEL_H

struct MeshUTFJoint {
 STDSTRINGW name;
 uint32 parent;
 real32 position[3];
 real32 m[9];
};

struct MeshUTFKeyFrame {
 uint32 frame;
 real32 position[3];
 real32 quaternion[4];
 real32 scale[3];
};

struct MeshUTFBoneKeyFrame {
 STDSTRINGW bone;
 uint32 bone_index;
 std::vector<MeshUTFKeyFrame> keyframes;
};

struct MeshUTFAnimation {
 STDSTRINGW name;
 std::vector<MeshUTFBoneKeyFrame> bonelist;
};

struct MeshUTFTexture {
 STDSTRINGW filename;
 uint32 channel;
};

struct MeshUTFMesh {
 STDSTRINGW name;
 std::vector<MeshUTFTexture> textures;
 std::vector<vector3D<real32>> position;
 std::vector<vector3D<real32>> normal;
 std::vector<vector2D<real32>> uv1;
 std::vector<vector2D<real32>> uv2;
 std::vector<vector4D<uint16>> bi;
 std::vector<vector4D<real32>> bw;
 std::vector<vector3D<real32>> color1;
 std::vector<vector3D<real32>> color2;
};

struct MeshUTFDirect3D {
  // Joint Buffers
  ID3D11Buffer* jd_buffer;
  ID3D11Buffer* jv_buffer;
  ID3D11Buffer* ji_buffer;
  ID3D11Buffer* jc_buffer;
  // Mesh Buffers
  ID3D11Buffer* mm_buffer;
};

class MeshUTF {
 private :
  std::vector<MeshUTFJoint> joints;
  std::vector<MeshUTFAnimation> animations;
  std::vector<MeshUTFMesh> meshes;
  std::vector<MeshUTFDirect3D> buffers;
 public :
  ErrorCode LoadModel(const wchar_t* filename);
  void FreeModel(void);
 public :
  ErrorCode RenderSkeleton(void);
  ErrorCode RenderMeshList(void);
  ErrorCode RenderModel(void);
 public : 
  MeshUTF();
  virtual ~MeshUTF();
 private :
  MeshUTF(const MeshUTF&) = delete;
  void operator =(const MeshUTF&) = delete;  
};

#endif
