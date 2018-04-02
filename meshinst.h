#ifndef __CS489_MESHINST_H
#define __CS489_MESHINST_H

#include "errors.h"
#include "model_v2.h"

class MeshInstance {
 private :
  const MeshData* mesh;
  real32 time;
  uint32 anim;
  bool loop;
 private :
  matrix4D mv;
  std::unique_ptr<matrix4D[]> jm;
 private :
  ID3D11Buffer* permodel;
  ID3D11Buffer* perframe;
 public :
  ErrorCode InitInstance(void);
  void FreeInstance(void);
 public :
  uint32 GetAnimation(void)const { return anim; }
  real32 GetTime(void)const { return time; }
 public :
  ErrorCode SetAnimation(uint32 index, bool repeat = false);
  ErrorCode SetTime(real32 value);
  ErrorCode ResetAnimation(void);
  ErrorCode Update(void);
  ErrorCode Update(real32 dt);
 public :
  ErrorCode RenderSkeleton(void);
  ErrorCode RenderModel(void);
 public :
  MeshInstance(const MeshData* mptr);
  MeshInstance(const MeshData* mptr, const real32* P, const real32* Q);
 ~MeshInstance();
 private :
  MeshInstance(const MeshInstance&) = delete;
  void operator =(const MeshInstance&) = delete;
};

#endif
