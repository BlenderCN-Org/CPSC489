#ifndef __CPSC489_MODEL_H
#define __CPSC489_MODEL_H

struct MeshUTFJoint {
 STDSTRINGW name;
 uint32 parent;
 real32 position[3];
 real32 m[9];
 matrix4D m_rel;
};

struct MeshUTFKeyFrame {
 uint32 frame;
 real32 translation[3];
 real32 quaternion[4];
 real32 scale[3];
};

struct MeshUTFBoneKeyFrame {
 STDSTRINGW bone;
 uint32 bone_index;
 uint32 minframe;
 uint32 maxframe;
 std::vector<MeshUTFKeyFrame> keyframes;
};

struct MeshUTFAnimationData {
 uint32 frame;
 real32 delta;
 std::unique_ptr<bool[]> keyed;
 std::unique_ptr<std::array<real32, 3>[]> slist;
 std::unique_ptr<std::array<real32, 3>[]> tlist;
 std::unique_ptr<std::array<real32, 4>[]> qlist;
 std::unique_ptr<matrix4D[]> mlist;
};

struct MeshUTFAnimation {
 STDSTRINGW name;
 bool loop;
 uint32 minframe;
 uint32 maxframe;
 real32 duration;
 std::set<uint32> keyset;
 std::map<uint32, size_t> keymap;
 std::vector<MeshUTFBoneKeyFrame> bonelist;
 std::unique_ptr<MeshUTFAnimationData[]> animdata;
};

struct MeshUTFTexture {
 STDSTRINGW semantic;
 uint32 channel;
 STDSTRINGW filename;
};

struct MeshUTFMesh {
 STDSTRINGW name;
 std::vector<MeshUTFTexture> textures;
 std::vector<std::array<real32, 3>> position;
 std::vector<std::array<real32, 3>> normal;
 std::vector<std::array<real32, 2>> uvs[2];
 std::vector<std::array<uint16, 4>> bi;
 std::vector<std::array<real32, 4>> bw;
 std::vector<std::array<real32, 3>> colors[2];
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
  friend class MeshUTFInstance;
 private :
  std::map<STDSTRINGW, uint32> jointmap;
  std::vector<MeshUTFJoint> joints;
  std::vector<MeshUTFAnimation> animations;
  std::vector<MeshUTFMesh> meshes;
  std::vector<MeshUTFDirect3D> buffers;
  ID3D11Buffer* ja_buffer;
 private :
  ErrorCode ConstructAnimationData(void);
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

class MeshUTFInstance {
 private :
  static const uint32 FRAMES_PER_SECOND = 30;
  const MeshUTF* mesh;
  real32 time;
  uint32 anim;
 private :
  DirectX::XMMATRIX m;
  std::unique_ptr<DirectX::XMMATRIX[]> jm;
 public :
  uint32 GetAnimation(void)const { return anim; }
  real32 GetTime(void)const { return time; }
 public :
  bool SetAnimation(uint32 index);
  bool SetTime(real32 value);
  bool ResetAnimation(void);
  bool Update(void);
  bool Update(real32 dt);
 public :
  MeshUTFInstance(const MeshUTF& ptr);
 private :
  MeshUTFInstance(const MeshUTFInstance&) = delete;
  void operator =(const MeshUTFInstance&) = delete;
};

#endif
