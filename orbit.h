#ifndef __CPSC489_ORBIT_H
#define __CPSC489_ORBIT_H

#include "errors.h"
#include "camera.h"

class OrbitBox {
 private :
  ID3D11Buffer* vbuffer;
  ID3D11Buffer* ibuffer;
  static const uint32 n_vertices = 8;
  static const uint32 vb_stride = 32;
  static const uint32 n_indices = 24;
  static const uint32 ib_stride = 2;
  static const bool scale_box = false;
 public :
  ErrorCode InitBox(const OrbitCamera& camera);
  void FreeBox(void);
  ErrorCode UpdateBox(const OrbitCamera& camera);
  ErrorCode RenderBox(void);
 public :
  OrbitBox();
 ~OrbitBox();
 public :
  OrbitBox(const OrbitBox& other) = delete;
  void operator =(const OrbitBox& other) = delete;
};

#endif
