#ifndef __CS489_AABB_H
#define __CS489_AABB_H

#include<iostream>
#include "vector3.h"

//
// MINMAX AABB
//
#pragma region AABB_MINMAX

struct AABB_minmax {

 //
 // DATA
 //
 public :
  float a[3];
  float b[3];

 //
 // QUERY
 //
 public :
  float dx(void)const { return b[0] - a[0]; }
  float dy(void)const { return b[1] - a[1]; }
  float dz(void)const { return b[2] - a[2]; }
  int dominator(void)const;
  int dominator(float* dims)const;
  int submissor(void)const;
  int submissor(float* dims)const;

 //
 // CONVERSION
 //
 public :
  void from(float a);
  void from(float a, float b);
  void from(float x1, float y1, float z1, float x2, float y2, float z2);
  void from(const float* A);
  void from(const float* A, const float* B);
  void from(const float* A, const float* B, const float* C);
  void from(const vector3D& A);
  void from(const vector3D& A, const vector3D& B);
  void from(const vector3D& A, const vector3D& B, const vector3D& C);
  void from(const AABB_minmax& A);
  void from(const AABB_minmax& A, const AABB_minmax& B);

 //
 // CONTRACTION/EXPANSION
 //
 public :
  void shrink(int index, float value);
  void shrink(float x, float y, float z);
  void shrink(const float* v);
  void shrink(const AABB_minmax& other);
  void grow(int index, float value);
  void grow(float x, float y, float z);
  void grow(const float* v);
  void grow(const AABB_minmax& other);

 //
 // CONSTRUCTORS
 //
 public :
  AABB_minmax();
  explicit AABB_minmax(float a);
  AABB_minmax(float a, float b);
  AABB_minmax(float x1, float y1, float z1, float x2, float y2, float z2);
  explicit AABB_minmax(const float* A);
  AABB_minmax(const float* A, const float* B);
  AABB_minmax(const float* A, const float* B, const float* C);
  AABB_minmax(const vector3D& A);
  AABB_minmax(const vector3D& A, const vector3D& B);
  AABB_minmax(const vector3D& A, const vector3D& B, const vector3D& C);
  AABB_minmax(const AABB_minmax& other);
};

 //
 // CONSTRUCTORS
 //

inline AABB_minmax::AABB_minmax()
{
}

inline AABB_minmax::AABB_minmax(float fill)
{
 from(fill);
}

inline AABB_minmax::AABB_minmax(float a, float b)
{
 from(a, b);
}

inline AABB_minmax::AABB_minmax(float x1, float y1, float z1, float x2, float y2, float z2)
{
 from(x1, y1, z1, x2, y2, z2);
}

inline AABB_minmax::AABB_minmax(const float* A)
{
 from(A);
}

inline AABB_minmax::AABB_minmax(const float* A, const float* B)
{
 from(A, B);
}

inline AABB_minmax::AABB_minmax(const float* A, const float* B, const float* C)
{
 from(A, B, C);
}

inline AABB_minmax::AABB_minmax(const vector3D& A)
{
 from(A);
}

inline AABB_minmax::AABB_minmax(const vector3D& A, const vector3D& B)
{
 from(A, B);
}

inline AABB_minmax::AABB_minmax(const vector3D& A, const vector3D& B, const vector3D& C)
{
 from(A, B, C);
}

inline AABB_minmax::AABB_minmax(const AABB_minmax& other)
{
 a[0] = other.a[0]; a[1] = other.a[1]; a[2] = other.a[2];
 b[0] = other.b[0]; b[1] = other.b[1]; b[2] = other.b[2];
}

//
// QUERY
//

inline int AABB_minmax::dominator(void)const
{
 float _dx = this->dx();
 float _dy = this->dy();
 float _dz = this->dz();
 if(_dx < _dy) return (_dy < _dz ? 2 : 1); // not dx
 return (_dx < _dz ? 2 : 0); // not dy
}

inline int AABB_minmax::dominator(float* dims)const
{
 float _dx = this->dx();
 float _dy = this->dy();
 float _dz = this->dz();
 if(dims) {
    dims[0] = _dx;
    dims[1] = _dy;
    dims[2] = _dz;
   }
 if(_dx < _dy) return (_dy < _dz ? 2 : 1); // not dx
 return (_dx < _dz ? 2 : 0); // not dy
}

inline int AABB_minmax::submissor(void)const
{
 float _dx = this->dx();
 float _dy = this->dy();
 float _dz = this->dz();
 if(_dz < _dy) return (_dz < _dx ? 2 : 0); // not dy
 return (_dy < _dx ? 1 : 0); // not dz
}

inline int AABB_minmax::submissor(float* dims)const
{
 float _dx = this->dx();
 float _dy = this->dy();
 float _dz = this->dz();
 if(dims) {
    dims[0] = _dx;
    dims[1] = _dy;
    dims[2] = _dz;
   }
 if(_dz < _dy) return (_dz < _dx ? 2 : 0); // not dy
 return (_dy < _dx ? 1 : 0); // not dz
}

//
// CONVERSION
//

inline void AABB_minmax::from(float a)
{
 this->a[0] = this->a[1] = this->a[2] = a;
 this->b[0] = this->b[1] = this->b[2] = a;
}

inline void AABB_minmax::from(float a, float b)
{
 this->a[0] = this->a[1] = this->a[2] = a;
 this->b[0] = this->b[1] = this->b[2] = b;
}

inline void AABB_minmax::from(float x1, float y1, float z1, float x2, float y2, float z2)
{
 if(x1 < x2) { a[0] = x1; b[0] = x2; } else { a[0] = x2; b[0] = x1; }
 if(y1 < y2) { a[1] = y1; b[1] = y2; } else { a[1] = y2; b[1] = y1; }
 if(z1 < z2) { a[2] = z1; b[2] = z2; } else { a[2] = z2; b[2] = z1; }
}

inline void AABB_minmax::from(const float* A)
{
 a[0] = b[0] = A[0];
 a[1] = b[1] = A[1];
 a[2] = b[2] = A[2];
}

inline void AABB_minmax::from(const float* A, const float* B)
{
 if(A[0] < B[0]) { a[0] = A[0]; b[0] = B[0]; } else { a[0] = B[0]; b[0] = A[0]; }
 if(A[1] < B[1]) { a[1] = A[1]; b[1] = B[1]; } else { a[1] = B[1]; b[1] = A[1]; }
 if(A[2] < B[2]) { a[2] = A[2]; b[2] = B[2]; } else { a[2] = B[2]; b[2] = A[2]; }
}

inline void AABB_minmax::from(const float* A, const float* B, const float* C)
{
 // test A
 a[0] = b[0] = A[0];
 a[1] = b[1] = A[1];
 a[2] = b[2] = A[2];
 // test B
 if(B[0] < a[0]) a[0] = B[0]; else if(b[0] < B[0]) b[0] = B[0];
 if(B[1] < a[1]) a[1] = B[1]; else if(b[1] < B[1]) b[1] = B[1];
 if(B[2] < a[2]) a[2] = B[2]; else if(b[2] < B[2]) b[2] = B[2];
 // test C
 if(C[0] < a[0]) a[0] = C[0]; else if(b[0] < C[0]) b[0] = C[0];
 if(C[1] < a[1]) a[1] = C[1]; else if(b[1] < C[1]) b[1] = C[1];
 if(C[2] < a[2]) a[2] = C[2]; else if(b[2] < C[2]) b[2] = C[2];
}

inline void AABB_minmax::from(const vector3D& A)
{
 a[0] = b[0] = A[0];
 a[1] = b[1] = A[1];
 a[2] = b[2] = A[2];
}

inline void AABB_minmax::from(const vector3D& A, const vector3D& B)
{
 if(A[0] < B[0]) { a[0] = A[0]; b[0] = B[0]; } else { a[0] = B[0]; b[0] = A[0]; }
 if(A[1] < B[1]) { a[1] = A[1]; b[1] = B[1]; } else { a[1] = B[1]; b[1] = A[1]; }
 if(A[2] < B[2]) { a[2] = A[2]; b[2] = B[2]; } else { a[2] = B[2]; b[2] = A[2]; }
}

inline void AABB_minmax::from(const vector3D& A, const vector3D& B, const vector3D& C)
{
 // test A
 a[0] = b[0] = A[0];
 a[1] = b[1] = A[1];
 a[2] = b[2] = A[2];
 // test B
 if(B[0] < a[0]) a[0] = B[0]; else if(b[0] < B[0]) b[0] = B[0];
 if(B[1] < a[1]) a[1] = B[1]; else if(b[1] < B[1]) b[1] = B[1];
 if(B[2] < a[2]) a[2] = B[2]; else if(b[2] < B[2]) b[2] = B[2];
 // test C
 if(C[0] < a[0]) a[0] = C[0]; else if(b[0] < C[0]) b[0] = C[0];
 if(C[1] < a[1]) a[1] = C[1]; else if(b[1] < C[1]) b[1] = C[1];
 if(C[2] < a[2]) a[2] = C[2]; else if(b[2] < C[2]) b[2] = C[2];
}

inline void AABB_minmax::from(const AABB_minmax& A)
{
 // set min (copy)
 a[0] = A.a[0];
 a[1] = A.a[1];
 a[2] = A.a[2];
 // set max (copy)
 b[0] = A.b[0];
 b[1] = A.b[1];
 b[2] = A.b[2];
}

inline void AABB_minmax::from(const AABB_minmax& A, const AABB_minmax& B)
{
 // compare minimums
 if(A.a[0] < B.a[0]) a[0] = A.a[0]; else a[0] = B.a[0];
 if(A.a[1] < B.a[1]) a[1] = A.a[1]; else a[1] = B.a[1];
 if(A.a[2] < B.a[2]) a[2] = A.a[2]; else a[2] = B.a[2];
 // compare maximums
 if(A.b[0] < B.b[0]) b[0] = B.b[0]; else b[0] = A.b[0];
 if(A.b[1] < B.b[1]) b[1] = B.b[1]; else b[1] = A.b[1];
 if(A.b[2] < B.b[2]) b[2] = B.b[2]; else b[2] = A.b[2];
}

//
// CONTRACTION/EXPANSION
//

inline void AABB_minmax::shrink(int index, float value)
{
}

inline void AABB_minmax::shrink(float x, float y, float z)
{
}

inline void AABB_minmax::shrink(const float* v)
{
}

inline void AABB_minmax::shrink(const AABB_minmax& other)
{
}

inline void AABB_minmax::grow(int index, float value)
{
 if(value < a[index]) a[index] = value;
 if(b[index] < value) b[index] = value;
}

inline void AABB_minmax::grow(float x, float y, float z)
{
 // do not assume a[i] is less than b[i]
 if(x < a[0]) a[0] = x; if(b[0] < x) b[0] = x;
 if(y < a[1]) a[1] = y; if(b[1] < y) b[0] = y;
 if(z < a[2]) a[2] = z; if(b[2] < z) b[0] = z;
}

inline void AABB_minmax::grow(const float* v)
{
 // do not assume a[i] is less than b[i]
 if(v[0] < a[0]) a[0] = v[0]; if(b[0] < v[0]) b[0] = v[0];
 if(v[1] < a[1]) a[1] = v[1]; if(b[1] < v[1]) b[0] = v[1];
 if(v[2] < a[2]) a[2] = v[2]; if(b[2] < v[2]) b[0] = v[2];
}

inline void AABB_minmax::grow(const AABB_minmax& other)
{
 // update min bounds
 if(other.a[0] < a[0]) a[0] = other.a[0];
 if(other.a[1] < a[1]) a[1] = other.a[1];
 if(other.a[2] < a[2]) a[2] = other.a[2];

 // update max bounds
 if(b[0] < other.b[0]) b[0] = other.b[0];
 if(b[1] < other.b[1]) b[1] = other.b[1];
 if(b[2] < other.b[2]) b[2] = other.b[2];
}

//
// NON-MEMBER FUNCTIONS
//

inline std::ostream& operator <<(std::ostream& os, const AABB_minmax& aabb)
{
 os << "min <" << aabb.a[0] << ", " << aabb.a[1] << ", " << aabb.a[2] << "> - ";
 os << "max <" << aabb.b[0] << ", " << aabb.b[1] << ", " << aabb.b[2] << ">";
 return os;
}

inline void centroid(const AABB_minmax& aabb, float* out)
{
 if(out) {
    out[0] = (aabb.a[0] + aabb.b[0])*0.5f;
    out[1] = (aabb.a[1] + aabb.b[1])*0.5f;
    out[2] = (aabb.a[2] + aabb.b[2])*0.5f;
   }
}

inline float half_surface_area(const AABB_minmax& aabb)
{
 float dx = aabb.dx();
 float dy = aabb.dy();
 float dz = aabb.dz();
 return dx*dy + dx*dz + dy*dz;
}

inline float surface_area(const AABB_minmax& aabb)
{
 float dx = aabb.dx();
 float dy = aabb.dy();
 float dz = aabb.dz();
 return 2.0f*(dx*dy + dx*dz + dy*dz);
}

inline float volume(const AABB_minmax& aabb)
{
 float dx = aabb.dx();
 float dy = aabb.dy();
 float dz = aabb.dz();
 return dx*dy*dz;
}

#pragma endregion AABB_MINMAX

//
// HALFDIM AABB
//
#pragma region AABB_HALFDIM

struct AABB_halfdim {

 //
 // DATA
 //
 public :
  float center[4];
  float widths[4];

};

//
// NON-MEMBER FUNCTIONS
//

inline void load_unit(AABB_halfdim& b)
{
 b.center[0] = 0.0f;
 b.center[1] = 0.0f;
 b.center[2] = 0.0f;
 b.widths[0] = 1.0f;
 b.widths[1] = 1.0f;
 b.widths[2] = 1.0f;
}

inline void scale(AABB_halfdim& b, float value)
{
 b.widths[0] *= value;
 b.widths[1] *= value;
 b.widths[2] *= value;
}

inline void scale(AABB_halfdim& b, const float* v)
{
 b.widths[0] *= v[0];
 b.widths[1] *= v[1];
 b.widths[2] *= v[2];
}

inline void translate(AABB_halfdim& b, const float* v)
{
 b.center[0] += v[0];
 b.center[1] += v[1];
 b.center[2] += v[2];
}

#pragma endregion AABB_HALFDIM

#endif
