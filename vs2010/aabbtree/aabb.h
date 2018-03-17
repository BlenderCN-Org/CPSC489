#ifndef __CS_AABB_H
#define __CS_AABB_H

struct AABB_minmax {

 //
 // DATA
 //
 public :
  float b_min[3];
  float b_max[3];

 //
 // QUERY
 //
 public :
  float dx(void)const { return b_max[0] - b_min[0]; }
  float dy(void)const { return b_max[1] - b_min[1]; }
  float dz(void)const { return b_max[2] - b_min[2]; }
  int dominator(void)const;
  int dominator(float* dims)const;
  int submissor(void)const;
  int submissor(float* dims)const;

 //
 // CONVERSION
 //
 public :
  void from(float fill);
  void from(float a, float b);
  void from(float x1, float y1, float z1, float x2, float y2, float z2);
  void from(const float* A);
  void from(const float* A, const float* B);
  void from(const float* A, const float* B, const float* C);

 //
 // CONTRACTION/EXPANSION
 //
 public :
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
  AABB_minmax(const AABB_minmax& other);
};

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

inline AABB_minmax::AABB_minmax(const AABB_minmax& other)
{
 b_min[0] = other.b_min[0]; b_min[1] = other.b_min[1]; b_min[2] = other.b_min[2];
 b_max[0] = other.b_max[0]; b_max[1] = other.b_max[1]; b_max[2] = other.b_max[2];
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
 b_min[0] = b_min[1] = b_min[2] = a;
 b_max[0] = b_max[1] = b_max[2] = a;
}

inline void AABB_minmax::from(float a, float b)
{
 if(a < b) {
    b_min[0] = b_min[1] = b_min[2] = a;
    b_max[0] = b_max[1] = b_max[2] = b;
   }
 else {
    b_min[0] = b_min[1] = b_min[2] = b;
    b_max[0] = b_max[1] = b_max[2] = a;
   }
}

inline void AABB_minmax::from(float x1, float y1, float z1, float x2, float y2, float z2)
{
 if(x1 < x2) { b_min[0] = x1; b_max[0] = x2; } else { b_min[0] = x2; b_max[0] = x1; }
 if(y1 < y2) { b_min[1] = y1; b_max[1] = y2; } else { b_min[1] = y2; b_max[1] = y1; }
 if(z1 < z2) { b_min[2] = z1; b_max[2] = z2; } else { b_min[2] = z2; b_max[2] = z1; }
}

inline void AABB_minmax::from(const float* A)
{
 b_min[0] = b_max[0] = A[0];
 b_min[1] = b_max[1] = A[1];
 b_min[2] = b_max[2] = A[2];
}

inline void AABB_minmax::from(const float* A, const float* B)
{
 if(A[0] < B[0]) { b_min[0] = A[0]; b_max[0] = B[0]; } else { b_min[0] = B[0]; b_max[0] = A[0]; }
 if(A[1] < B[1]) { b_min[1] = A[1]; b_max[1] = B[1]; } else { b_min[1] = B[1]; b_max[1] = A[1]; }
 if(A[2] < B[2]) { b_min[2] = A[2]; b_max[2] = B[2]; } else { b_min[2] = B[2]; b_max[2] = A[2]; }
}

inline void AABB_minmax::from(const float* A, const float* B, const float* C)
{
 // test A
 b_min[0] = b_max[0] = A[0];
 b_min[1] = b_max[1] = A[1];
 b_min[2] = b_max[2] = A[2];
 // test B
 if(B[0] < b_min[0]) b_min[0] = B[0]; else if(b_max[0] < B[0]) b_max[0] = B[0];
 if(B[1] < b_min[1]) b_min[1] = B[1]; else if(b_max[1] < B[1]) b_max[1] = B[1];
 if(B[2] < b_min[2]) b_min[2] = B[2]; else if(b_max[2] < B[2]) b_max[2] = B[2];
 // test C
 if(C[0] < b_min[0]) b_min[0] = C[0]; else if(b_max[0] < C[0]) b_max[0] = C[0];
 if(C[1] < b_min[1]) b_min[1] = C[1]; else if(b_max[1] < C[1]) b_max[1] = C[1];
 if(C[2] < b_min[2]) b_min[2] = C[2]; else if(b_max[2] < C[2]) b_max[2] = C[2];
}

inline void AABB_minmax::grow(int index, float value)
{
 if(value < b_min[index]) b_min[index] = value;
 if(b_max[index] < value) b_max[index] = value;
}

inline void AABB_minmax::grow(float x, float y, float z)
{
 // do not assume b_min[i] is less than b_max[i]
 if(x < b_min[0]) b_min[0] = x; if(b_max[0] < x) b_max[0] = x;
 if(y < b_min[1]) b_min[1] = y; if(b_max[1] < y) b_max[0] = y;
 if(z < b_min[2]) b_min[2] = z; if(b_max[2] < z) b_max[0] = z;
}

inline void AABB_minmax::grow(const float* v)
{
 // do not assume b_min[i] is less than b_max[i]
 if(v[0] < b_min[0]) b_min[0] = v[0]; if(b_max[0] < v[0]) b_max[0] = v[0];
 if(v[1] < b_min[1]) b_min[1] = v[1]; if(b_max[1] < v[1]) b_max[0] = v[1];
 if(v[2] < b_min[2]) b_min[2] = v[2]; if(b_max[2] < v[2]) b_max[0] = v[2];
}

inline void AABB_minmax::grow(const AABB_minmax& other)
{
 // update min bounds
 if(other.b_min[0] < b_min[0]) b_min[0] = other.b_min[0];
 if(other.b_min[1] < b_min[1]) b_min[1] = other.b_min[1];
 if(other.b_min[2] < b_min[2]) b_min[2] = other.b_min[2];

 // update max bounds
 if(b_max[0] < other.b_max[0]) b_max[0] = other.b_max[0];
 if(b_max[1] < other.b_max[1]) b_max[1] = other.b_max[1];
 if(b_max[2] < other.b_max[2]) b_max[2] = other.b_max[2];
}

//
// NON-MEMBER FUNCTIONS
//

inline void centroid(const AABB_minmax& aabb, float* out)
{
 if(out) {
    out[0] = (aabb.b_min[0] + aabb.b_max[0])*0.5f;
    out[1] = (aabb.b_min[1] + aabb.b_max[1])*0.5f;
    out[2] = (aabb.b_min[2] + aabb.b_max[2])*0.5f;
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

#endif
