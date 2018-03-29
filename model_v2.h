#ifndef __CS489_MODEL_V2_H
#define __CS489_MODEL_V2_H

#include "matrix4.h"

class MeshData {
  friend class MeshInstance;
  typedef struct _c_point2D { real32 v[2]; } c_point2D;
  typedef struct _c_point3D { real32 v[3]; } c_point3D;
  typedef struct _c_color4D { real32 v[4]; } c_color4D;
  typedef struct _c_blend4i { uint16 v[8]; } c_blend4i;
  typedef struct _c_blend4w { real32 v[8]; } c_blend4w;
  typedef struct _c_triface { uint32 v[3]; } c_triface;
  static const uint16 DIFFUSE_MAP = 0;
 private :
  struct MeshBone {
   STDSTRINGW name;
   uint32 parent;
   real32 position[3];
   matrix4D m_abs;
   matrix4D m_inv;
   matrix4D m_rel;
  };
  struct MeshKeyFrame {
   uint32 frame;
   real32 translation[3];
   real32 quaternion[4];
   real32 scale[3];
  };
  struct MeshAnimatedBoneKeys {
   uint32 bone_index;
   uint32 minframe;
   uint32 maxframe;
   std::vector<MeshKeyFrame> keyframes;
  };
  struct MeshAnimationData {
   uint32 frame;
   real32 delta;
   std::unique_ptr<bool[]> keyed;
   std::unique_ptr<std::array<real32, 3>[]> slist;
   std::unique_ptr<std::array<real32, 3>[]> tlist;
   std::unique_ptr<std::array<real32, 4>[]> qlist;
   std::unique_ptr<matrix4D[]> mlist;
  };
  struct MeshAnimation {
   STDSTRINGW name;
   bool loop;
   uint32 minframe;
   uint32 maxframe;
   real32 duration;
   std::set<uint32> keyset;
   std::map<uint32, size_t> keymap;
   std::vector<MeshAnimatedBoneKeys> bonelist;
   std::unique_ptr<MeshAnimationData[]> animdata;
  };
  struct MeshTexture {
   STDSTRINGW filename;
   uint32 semantic;
   uint16 uv_index;
   uint16 resource;
   struct TextureProperties {
   };
  };
  struct MeshMaterial {
   STDSTRINGW name;
   std::vector<MeshTexture> textures;
  };
  struct MeshCollision {
   uint32 n_verts;
   uint32 n_faces;
   std::unique_ptr<c_point3D[]> position;
   std::unique_ptr<c_triface[]> facelist;
  };
  struct MeshSurface {
   uint32 n_faces;
   uint32 m_index;
   uint32 start;
   std::unique_ptr<c_triface[]> facelist;
  };
  struct MeshBuffers {
   STDSTRINGW name;
   std::vector<MeshMaterial> materials;
   uint32 n_verts;
   uint32 n_faces;
   std::unique_ptr<c_point3D[]> position;
   std::unique_ptr<c_point3D[]> normal;
   std::unique_ptr<c_point2D[]> uvs[2];
   std::unique_ptr<c_blend4i[]> bi;
   std::unique_ptr<c_blend4w[]> bw;
   std::unique_ptr<c_color4D[]> colors[2];
   std::vector<MeshSurface> surfaces;
  };
  struct MeshGraphics {
    std::unique_ptr<ID3D11Buffer*[]> vbuffer;
    std::unique_ptr<ID3D11Buffer*[]> ibuffer;
    ID3D11Buffer* jbuffer;
    std::vector<ID3D11ShaderResourceView*> resources;
  };
 private :
  bool skeletal;
  std::map<STDSTRINGW, uint32> bonemap;
  std::vector<MeshBone> bones;
  std::vector<MeshAnimation> animations;
  std::vector<MeshCollision> collisions;
  std::vector<MeshBuffers> meshes;
  MeshGraphics graphics;
 private :
  void ConstructAnimationData(void);
  bool ConstructGraphics(void);
 public :
  bool LoadMeshUTF(const wchar_t* filename);
  bool LoadMeshBIN(const wchar_t* filename);
  void Free(void);
 public :
  bool SaveMeshUTF(const wchar_t* filename);
  bool SaveMeshBIN(const wchar_t* filename);
 public : 
  MeshData();
  virtual ~MeshData();
 private :
  MeshData(const MeshData&) = delete;
  void operator =(const MeshData&) = delete;  
};

#endif
