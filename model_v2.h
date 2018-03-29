#ifndef __CS489_MODEL_V2_H
#define __CS489_MODEL_V2_H

#include "matrix4.h"

class MeshData {
  friend class MeshInstance;
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
   uint32 uv_index;
   struct TextureProperties {
   };
  };
  struct MeshMaterial {
   STDSTRINGW name;
   std::vector<MeshTexture> textures;
  };
  struct MeshBuffers {
   STDSTRINGW name;
   std::vector<MeshMaterial> materials;
   uint32 n_verts;
   std::unique_ptr<std::array<real32, 3>[]> position;
   std::unique_ptr<std::array<real32, 3>[]> normal;
   std::unique_ptr<std::array<real32, 2>[]> uvs[2];
   std::unique_ptr<std::array<uint16, 4>[]> bi;
   std::unique_ptr<std::array<real32, 4>[]> bw;
   std::unique_ptr<std::array<real32, 3>[]> colors[2];
   uint32 n_faces;
   std::unique_ptr<uint32[]> facelist;
  };
  struct MeshGraphics {
    ID3D11Buffer* vbuffer;
    ID3D11Buffer* ibuffer;
    ID3D11Buffer* jbuffer;
    std::unique_ptr<ID3D11ShaderResourceView*[]> rvlist;
  };
 private :
  std::map<STDSTRINGW, uint32> bonemap;
  std::vector<MeshBone> bones;
  std::vector<MeshAnimation> animations;
  std::vector<MeshBuffers> meshes;
  std::unique_ptr<MeshGraphics[]> buffers;
  bool has_weights;
 private :
 private :
  bool ConstructAnimationData(void);
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
