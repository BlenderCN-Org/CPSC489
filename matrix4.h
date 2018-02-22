#ifndef __CPSC489_MATRIX4_H
#define __CPSC489_MATRIX4_H

class matrix4D;

#pragma region MATRIX4D_CLASS_DEFINITIONS

typedef struct _c_smatrix4D { real32 m[16]; } c_smatrix4D;
class matrix4D : public c_smatrix4D {
 public :
  static const int m11 = 0x0;
  static const int m12 = 0x1;
  static const int m13 = 0x2;
  static const int m14 = 0x3;
  static const int m21 = 0x4;
  static const int m22 = 0x5;
  static const int m23 = 0x6;
  static const int m24 = 0x7;
  static const int m31 = 0x8;
  static const int m32 = 0x9;
  static const int m33 = 0xA;
  static const int m34 = 0xB;
  static const int m41 = 0xC;
  static const int m42 = 0xD;
  static const int m43 = 0xE;
  static const int m44 = 0xF;
 public :
  typedef real32 value_type;
  typedef real32 array_type[16];
 public :
  matrix4D();
  explicit matrix4D(real32 fill);
  explicit matrix4D(const real32* m);
  matrix4D(const matrix4D& other);
 public :
  real32* data(void) { return &(this->m[0]); }
  const real32* data(void)const { return &(this->m[0]); }
  array_type& get(void) { return this->m; }
  const array_type& get(void)const { return this->m; }
 public :
  const real32& at(int r, int c)const;
  real32& at(int r, int c);
  real32 determinant(void)const;
  real32 invert(void);
  void transpose(void);
 public :
  void load_identity(void);
  void load_scaling(real32 x, real32 y, real32 z);
  void load_translation(real32 x, real32 y, real32 z);
  void load_quaternion(real32 w, real32 x, real32 y, real32 z);
  void load_quaternion(const real32* q);
 public :
  matrix4D& premul(const matrix4D& A);
  matrix4D& mul(const matrix4D& A);
 public :
  const real32& operator [](size_t index)const;
  real32& operator [](size_t index);
 public :
  const real32& operator ()(int r, int c)const;
  real32& operator ()(int r, int c);
 public :
  matrix4D& operator =(const matrix4D& other);
 public :
  matrix4D& operator *=(real32 scalar);
  matrix4D& operator /=(real32 scalar);
 public :
  matrix4D& operator +=(const matrix4D& other);
  matrix4D& operator -=(const matrix4D& other);
  matrix4D& operator *=(const matrix4D& other);
};

inline matrix4D::matrix4D()
{
}

inline matrix4D::matrix4D(real32 fill)
{
 this->m[ 0] = this->m[ 1] = this->m[ 2] = this->m[ 3] = fill;
 this->m[ 4] = this->m[ 5] = this->m[ 6] = this->m[ 7] = fill;
 this->m[ 8] = this->m[ 9] = this->m[10] = this->m[11] = fill;
 this->m[12] = this->m[13] = this->m[14] = this->m[15] = fill;
}

inline matrix4D::matrix4D(const real32* m)
{
 this->m[ 0] = m[ 0];
 this->m[ 1] = m[ 1];
 this->m[ 2] = m[ 2];
 this->m[ 3] = m[ 3];
 this->m[ 4] = m[ 4];
 this->m[ 5] = m[ 5];
 this->m[ 6] = m[ 6];
 this->m[ 7] = m[ 7];
 this->m[ 8] = m[ 8];
 this->m[ 9] = m[ 9];
 this->m[10] = m[10];
 this->m[11] = m[11];
 this->m[12] = m[12];
 this->m[13] = m[13];
 this->m[14] = m[14];
 this->m[15] = m[15];
}

inline matrix4D::matrix4D(const matrix4D& other)
{
 this->m[ 0] = other.m[ 0];
 this->m[ 1] = other.m[ 1];
 this->m[ 2] = other.m[ 2];
 this->m[ 3] = other.m[ 3];
 this->m[ 4] = other.m[ 4];
 this->m[ 5] = other.m[ 5];
 this->m[ 6] = other.m[ 6];
 this->m[ 7] = other.m[ 7];
 this->m[ 8] = other.m[ 8];
 this->m[ 9] = other.m[ 9];
 this->m[10] = other.m[10];
 this->m[11] = other.m[11];
 this->m[12] = other.m[12];
 this->m[13] = other.m[13];
 this->m[14] = other.m[14];
 this->m[15] = other.m[15];
}

inline const real32& matrix4D::at(int r, int c)const
{
 if(r < 0 || r > 3) throw std::out_of_range("matrix4D<real32> index out of range.");
 return this->m[(r << 2) + c];
}

inline real32& matrix4D::at(int r, int c)
{
 if(r < 0 || r > 3) throw std::out_of_range("matrix4D<real32> index out of range.");
 return this->m[(r << 2) + c];
}

inline real32 matrix4D::determinant(void)const
{
 // cache 2x2 determinants
 real32 D[12] = {
  this->m[m33]*this->m[m44] - this->m[m34]*this->m[m43],
  this->m[m32]*this->m[m44] - this->m[m34]*this->m[m42],
  this->m[m32]*this->m[m43] - this->m[m33]*this->m[m42],
  this->m[m31]*this->m[m44] - this->m[m34]*this->m[m41],
  this->m[m31]*this->m[m43] - this->m[m33]*this->m[m41],
  this->m[m31]*this->m[m42] - this->m[m32]*this->m[m41],
  this->m[m13]*this->m[m24] - this->m[m14]*this->m[m23],
  this->m[m12]*this->m[m24] - this->m[m14]*this->m[m22],
  this->m[m12]*this->m[m23] - this->m[m13]*this->m[m22],
  this->m[m11]*this->m[m24] - this->m[m14]*this->m[m21],
  this->m[m11]*this->m[m23] - this->m[m13]*this->m[m21],
  this->m[m11]*this->m[m22] - this->m[m12]*this->m[m21] 
 };

 // compute determinant
 return D[0x5]*D[0x6] - D[0x4]*D[0x7] + D[0x3]*D[0x8] + D[0x2]*D[0x9] - D[0x1]*D[0xA] + D[0x0]*D[0xB];
}

inline real32 matrix4D::invert(void)
{
 // cache 2x2 determinants
 real32 D[12] = {
  sarrus_2x2(this->m[m33], this->m[m34], this->m[m43], this->m[m44]),
  sarrus_2x2(this->m[m32], this->m[m34], this->m[m42], this->m[m44]),
  sarrus_2x2(this->m[m32], this->m[m33], this->m[m42], this->m[m43]),
  sarrus_2x2(this->m[m31], this->m[m34], this->m[m41], this->m[m44]),
  sarrus_2x2(this->m[m31], this->m[m33], this->m[m41], this->m[m43]),
  sarrus_2x2(this->m[m31], this->m[m32], this->m[m41], this->m[m42]),
  sarrus_2x2(this->m[m13], this->m[m14], this->m[m23], this->m[m24]),
  sarrus_2x2(this->m[m12], this->m[m14], this->m[m22], this->m[m24]),
  sarrus_2x2(this->m[m12], this->m[m13], this->m[m22], this->m[m23]),
  sarrus_2x2(this->m[m11], this->m[m14], this->m[m21], this->m[m24]),
  sarrus_2x2(this->m[m11], this->m[m13], this->m[m21], this->m[m23]),
  sarrus_2x2(this->m[m11], this->m[m12], this->m[m21], this->m[m22])
 };

 // compute determinant
 real32 det = D[0x5]*D[0x6] - D[0x4]*D[0x7] + D[0x3]*D[0x8] + D[0x2]*D[0x9] - D[0x1]*D[0xA] + D[0x0]*D[0xB];
 if(is_zero(det)) return det;

 // compute cofactor matrix
 real32 C[16] = {
  +(this->m[m22]*D[0x0] - this->m[m23]*D[0x1] + this->m[m24]*D[0x2]),
  -(this->m[m21]*D[0x0] - this->m[m23]*D[0x3] + this->m[m24]*D[0x4]),
  +(this->m[m21]*D[0x1] - this->m[m22]*D[0x3] + this->m[m24]*D[0x5]),
  -(this->m[m21]*D[0x2] - this->m[m22]*D[0x4] + this->m[m23]*D[0x5]),
  -(this->m[m12]*D[0x0] - this->m[m13]*D[0x1] + this->m[m14]*D[0x2]),
  +(this->m[m11]*D[0x0] - this->m[m13]*D[0x3] + this->m[m14]*D[0x4]),
  -(this->m[m11]*D[0x1] - this->m[m12]*D[0x3] + this->m[m14]*D[0x5]),
  +(this->m[m11]*D[0x2] - this->m[m12]*D[0x4] + this->m[m13]*D[0x5]),
  +(this->m[m42]*D[0x6] - this->m[m43]*D[0x7] + this->m[m44]*D[0x8]),
  -(this->m[m41]*D[0x6] - this->m[m43]*D[0x9] + this->m[m44]*D[0xA]),
  +(this->m[m41]*D[0x7] - this->m[m42]*D[0x9] + this->m[m44]*D[0xB]),
  -(this->m[m41]*D[0x8] - this->m[m42]*D[0xA] + this->m[m43]*D[0xB]),
  -(this->m[m32]*D[0x6] - this->m[m33]*D[0x7] + this->m[m34]*D[0x8]),
  +(this->m[m31]*D[0x6] - this->m[m33]*D[0x9] + this->m[m34]*D[0xA]),
  -(this->m[m31]*D[0x7] - this->m[m32]*D[0x9] + this->m[m34]*D[0xB]),
  +(this->m[m31]*D[0x8] - this->m[m32]*D[0xA] + this->m[m33]*D[0xB]) 
 };

 // compute inverse matrix inv(M) = inv(det(M)) * transpose(C)
 real32 invdet = inv(det);
 this->m[m11] = C[m11]*invdet;
 this->m[m21] = C[m12]*invdet;
 this->m[m31] = C[m13]*invdet;
 this->m[m41] = C[m14]*invdet;
 this->m[m12] = C[m21]*invdet;
 this->m[m22] = C[m22]*invdet;
 this->m[m32] = C[m23]*invdet;
 this->m[m42] = C[m24]*invdet;
 this->m[m13] = C[m31]*invdet;
 this->m[m23] = C[m32]*invdet;
 this->m[m33] = C[m33]*invdet;
 this->m[m43] = C[m34]*invdet;
 this->m[m14] = C[m41]*invdet;
 this->m[m24] = C[m42]*invdet;
 this->m[m34] = C[m43]*invdet;
 this->m[m44] = C[m44]*invdet;

 // return determinant
 return det;
}

inline void matrix4D::transpose(void)
{
 real32 temp;
 temp = this->m[m12]; this->m[m12] = this->m[m21]; this->m[m21] = temp;
 temp = this->m[m13]; this->m[m13] = this->m[m31]; this->m[m31] = temp;
 temp = this->m[m14]; this->m[m14] = this->m[m41]; this->m[m41] = temp;
 temp = this->m[m23]; this->m[m23] = this->m[m32]; this->m[m32] = temp;
 temp = this->m[m24]; this->m[m24] = this->m[m42]; this->m[m42] = temp;
 temp = this->m[m34]; this->m[m34] = this->m[m43]; this->m[m43] = temp;
}

inline void matrix4D::load_identity(void)
{
 this->m[m11] = 1.0f; this->m[m21] = 0.0f; this->m[m31] = 0.0f; this->m[m41] = 0.0f;
 this->m[m12] = 0.0f; this->m[m22] = 1.0f; this->m[m32] = 0.0f; this->m[m42] = 0.0f;
 this->m[m13] = 0.0f; this->m[m23] = 0.0f; this->m[m33] = 1.0f; this->m[m43] = 0.0f;
 this->m[m14] = 0.0f; this->m[m24] = 0.0f; this->m[m34] = 0.0f; this->m[m44] = 1.0f;
}

inline void matrix4D::load_scaling(real32 x, real32 y, real32 z)
{
 this->m[m11] =    x; this->m[m21] = 0.0f; this->m[m31] = 0.0f; this->m[m41] = 0.0f;
 this->m[m12] = 0.0f; this->m[m22] =    y; this->m[m32] = 0.0f; this->m[m42] = 0.0f;
 this->m[m13] = 0.0f; this->m[m23] = 0.0f; this->m[m33] =    z; this->m[m43] = 0.0f;
 this->m[m14] = 0.0f; this->m[m24] = 0.0f; this->m[m34] = 0.0f; this->m[m44] = 1.0f;
}

inline void matrix4D::load_translation(real32 x, real32 y, real32 z)
{
 this->m[m11] = 1.0f; this->m[m21] = 0.0f; this->m[m31] = 0.0f; this->m[m41] = x;
 this->m[m12] = 0.0f; this->m[m22] = 1.0f; this->m[m32] = 0.0f; this->m[m42] = y;
 this->m[m13] = 0.0f; this->m[m23] = 0.0f; this->m[m33] = 1.0f; this->m[m43] = z;
 this->m[m14] = 0.0f; this->m[m24] = 0.0f; this->m[m34] = 0.0f; this->m[m44] = 1.0f;
}

inline void matrix4D::load_quaternion(real32 w, real32 x, real32 y, real32 z)
{
 // constants
 real32 zer = 0.0f;
 real32 one = 1.0f;

 // factors
 real32 a2 = 2*w;
 real32 b2 = 2*x;
 real32 c2 = 2*y;
 real32 d2 = 2*z;

 // factors
 real32 ab2 = w * b2;
 real32 ac2 = w * c2;
 real32 ad2 = w * d2;
 real32 bb2 = x * b2;
 real32 bc2 = x * c2;
 real32 bd2 = x * d2;
 real32 cc2 = y * c2;
 real32 cd2 = y * d2;
 real32 dd2 = z * d2;

 // reinterpret matrix
 this->m[0x0] = one - cc2 - dd2; // 1 - 2yy - 2zz
 this->m[0x1] = bc2 - ad2;       // 2xy - 2wz
 this->m[0x2] = bd2 + ac2;       // 2xz + 2wy
 this->m[0x3] = zer;
 this->m[0x4] = bc2 + ad2;
 this->m[0x5] = one - bb2 - dd2;
 this->m[0x6] = cd2 - ab2;
 this->m[0x7] = zer;
 this->m[0x8] = bd2 - ac2;
 this->m[0x9] = cd2 + ab2;
 this->m[0xA] = one - bb2 - cc2;
 this->m[0xB] = zer;
 this->m[0xC] = zer;
 this->m[0xD] = zer;
 this->m[0xE] = zer;
 this->m[0xF] = one;
}

inline void matrix4D::load_quaternion(const real32* q)
{
 // constants
 real32 zer = 0.0f;
 real32 one = 1.0f;

 // factors
 real32 a2 = q[0] + q[0];
 real32 b2 = q[1] + q[1];
 real32 c2 = q[2] + q[2];
 real32 d2 = q[3] + q[3];

 // factors
 real32 ab2 = q[0] * b2;
 real32 ac2 = q[0] * c2;
 real32 ad2 = q[0] * d2;
 real32 bb2 = q[1] * b2;
 real32 bc2 = q[1] * c2;
 real32 bd2 = q[1] * d2;
 real32 cc2 = q[2] * c2;
 real32 cd2 = q[2] * d2;
 real32 dd2 = q[3] * d2;

 // reinterpret matrix
 this->m[0x0] = one - cc2 - dd2;
 this->m[0x1] = bc2 - ad2;
 this->m[0x2] = bd2 + ac2;
 this->m[0x3] = zer;
 this->m[0x4] = bc2 + ad2;
 this->m[0x5] = one - bb2 - dd2;
 this->m[0x6] = cd2 - ab2;
 this->m[0x7] = zer;
 this->m[0x8] = bd2 - ac2;
 this->m[0x9] = cd2 + ab2;
 this->m[0xA] = one - bb2 - cc2;
 this->m[0xB] = zer;
 this->m[0xC] = zer;
 this->m[0xD] = zer;
 this->m[0xE] = zer;
 this->m[0xF] = one;
}

inline matrix4D& matrix4D::premul(const matrix4D& A)
{
 // computes (*this) = A x (*this)
 real32 X[16] = {
  A[0x0]*this->m[0x0] + A[0x1]*this->m[0x4] + A[0x2]*this->m[0x8] + A[0x3]*this->m[0xC],
  A[0x0]*this->m[0x1] + A[0x1]*this->m[0x5] + A[0x2]*this->m[0x9] + A[0x3]*this->m[0xD],
  A[0x0]*this->m[0x2] + A[0x1]*this->m[0x6] + A[0x2]*this->m[0xA] + A[0x3]*this->m[0xE],
  A[0x0]*this->m[0x3] + A[0x1]*this->m[0x7] + A[0x2]*this->m[0xB] + A[0x3]*this->m[0xF],
  A[0x4]*this->m[0x0] + A[0x5]*this->m[0x4] + A[0x6]*this->m[0x8] + A[0x7]*this->m[0xC],
  A[0x4]*this->m[0x1] + A[0x5]*this->m[0x5] + A[0x6]*this->m[0x9] + A[0x7]*this->m[0xD],
  A[0x4]*this->m[0x2] + A[0x5]*this->m[0x6] + A[0x6]*this->m[0xA] + A[0x7]*this->m[0xE],
  A[0x4]*this->m[0x3] + A[0x5]*this->m[0x7] + A[0x6]*this->m[0xB] + A[0x7]*this->m[0xF],
  A[0x8]*this->m[0x0] + A[0x9]*this->m[0x4] + A[0xA]*this->m[0x8] + A[0xB]*this->m[0xC],
  A[0x8]*this->m[0x1] + A[0x9]*this->m[0x5] + A[0xA]*this->m[0x9] + A[0xB]*this->m[0xD],
  A[0x8]*this->m[0x2] + A[0x9]*this->m[0x6] + A[0xA]*this->m[0xA] + A[0xB]*this->m[0xE],
  A[0x8]*this->m[0x3] + A[0x9]*this->m[0x7] + A[0xA]*this->m[0xB] + A[0xB]*this->m[0xF],
  A[0xC]*this->m[0x0] + A[0xD]*this->m[0x4] + A[0xE]*this->m[0x8] + A[0xF]*this->m[0xC],
  A[0xC]*this->m[0x1] + A[0xD]*this->m[0x5] + A[0xE]*this->m[0x9] + A[0xF]*this->m[0xD],
  A[0xC]*this->m[0x2] + A[0xD]*this->m[0x6] + A[0xE]*this->m[0xA] + A[0xF]*this->m[0xE],
  A[0xC]*this->m[0x3] + A[0xD]*this->m[0x7] + A[0xE]*this->m[0xB] + A[0xF]*this->m[0xF]
 };
 this->m[0x0] = X[0x0];
 this->m[0x1] = X[0x1];
 this->m[0x2] = X[0x2];
 this->m[0x3] = X[0x3];
 this->m[0x4] = X[0x4];
 this->m[0x5] = X[0x5];
 this->m[0x6] = X[0x6];
 this->m[0x7] = X[0x7];
 this->m[0x8] = X[0x8];
 this->m[0x9] = X[0x9];
 this->m[0xA] = X[0xA];
 this->m[0xB] = X[0xB];
 this->m[0xC] = X[0xC];
 this->m[0xD] = X[0xD];
 this->m[0xE] = X[0xE];
 this->m[0xF] = X[0xF];

 return *this;
}

inline matrix4D& matrix4D::mul(const matrix4D& A)
{
 // computes (*this) = (*this) x A
 return ((*this) *= A);
}

inline const real32& matrix4D::operator [](size_t index)const
{
 return this->m[index];
}

inline real32& matrix4D::operator [](size_t index)
{
 return this->m[index];
}

inline const real32& matrix4D::operator ()(int r, int c)const
{
 return this->m[(r << 2) + c];
}

inline real32& matrix4D::operator ()(int r, int c)
{
 return this->m[(r << 2) + c];
}

inline matrix4D& matrix4D::operator =(const matrix4D& other)
{
 if(this == &other) return *this;
 this->m[ 0] = other.m[ 0];
 this->m[ 1] = other.m[ 1];
 this->m[ 2] = other.m[ 2];
 this->m[ 3] = other.m[ 3];
 this->m[ 4] = other.m[ 4];
 this->m[ 5] = other.m[ 5];
 this->m[ 6] = other.m[ 6];
 this->m[ 7] = other.m[ 7];
 this->m[ 8] = other.m[ 8];
 this->m[ 9] = other.m[ 9];
 this->m[10] = other.m[10];
 this->m[11] = other.m[11];
 this->m[12] = other.m[12];
 this->m[13] = other.m[13];
 this->m[14] = other.m[14];
 this->m[15] = other.m[15];
 return *this;
}

inline matrix4D& matrix4D::operator *=(real32 scalar)
{
 this->m[ 0] *= scalar;
 this->m[ 1] *= scalar;
 this->m[ 2] *= scalar;
 this->m[ 3] *= scalar;
 this->m[ 4] *= scalar;
 this->m[ 5] *= scalar;
 this->m[ 6] *= scalar;
 this->m[ 7] *= scalar;
 this->m[ 8] *= scalar;
 this->m[ 9] *= scalar;
 this->m[10] *= scalar;
 this->m[11] *= scalar;
 this->m[12] *= scalar;
 this->m[13] *= scalar;
 this->m[14] *= scalar;
 this->m[15] *= scalar;
 return *this;
}

inline matrix4D& matrix4D::operator /=(real32 scalar)
{
 this->m[ 0] /= scalar;
 this->m[ 1] /= scalar;
 this->m[ 2] /= scalar;
 this->m[ 3] /= scalar;
 this->m[ 4] /= scalar;
 this->m[ 5] /= scalar;
 this->m[ 6] /= scalar;
 this->m[ 7] /= scalar;
 this->m[ 8] /= scalar;
 this->m[ 9] /= scalar;
 this->m[10] /= scalar;
 this->m[11] /= scalar;
 this->m[12] /= scalar;
 this->m[13] /= scalar;
 this->m[14] /= scalar;
 this->m[15] /= scalar;
 return *this;
}

inline matrix4D& matrix4D::operator +=(const matrix4D& other)
{
 this->m[ 0] += other.m[ 0];
 this->m[ 1] += other.m[ 1];
 this->m[ 2] += other.m[ 2];
 this->m[ 3] += other.m[ 3];
 this->m[ 4] += other.m[ 4];
 this->m[ 5] += other.m[ 5];
 this->m[ 6] += other.m[ 6];
 this->m[ 7] += other.m[ 7];
 this->m[ 8] += other.m[ 8];
 this->m[ 9] += other.m[ 9];
 this->m[10] += other.m[10];
 this->m[11] += other.m[11];
 this->m[12] += other.m[12];
 this->m[13] += other.m[13];
 this->m[14] += other.m[14];
 this->m[15] += other.m[15];
 return *this;
}

inline matrix4D& matrix4D::operator -=(const matrix4D& other)
{
 this->m[ 0] -= other.m[ 0];
 this->m[ 1] -= other.m[ 1];
 this->m[ 2] -= other.m[ 2];
 this->m[ 3] -= other.m[ 3];
 this->m[ 4] -= other.m[ 4];
 this->m[ 5] -= other.m[ 5];
 this->m[ 6] -= other.m[ 6];
 this->m[ 7] -= other.m[ 7];
 this->m[ 8] -= other.m[ 8];
 this->m[ 9] -= other.m[ 9];
 this->m[10] -= other.m[10];
 this->m[11] -= other.m[11];
 this->m[12] -= other.m[12];
 this->m[13] -= other.m[13];
 this->m[14] -= other.m[14];
 this->m[15] -= other.m[15];
 return *this;
}

inline matrix4D& matrix4D::operator *=(const matrix4D& other)
{
 real32 temp[16] = {
  this->m[ 0]*other.m[ 0] + this->m[ 1]*other.m[ 4] + this->m[ 2]*other.m[ 8] + this->m[ 3]*other.m[12],
  this->m[ 0]*other.m[ 1] + this->m[ 1]*other.m[ 5] + this->m[ 2]*other.m[ 9] + this->m[ 3]*other.m[13],
  this->m[ 0]*other.m[ 2] + this->m[ 1]*other.m[ 6] + this->m[ 2]*other.m[10] + this->m[ 3]*other.m[14],
  this->m[ 0]*other.m[ 3] + this->m[ 1]*other.m[ 7] + this->m[ 2]*other.m[11] + this->m[ 3]*other.m[15],
  this->m[ 4]*other.m[ 0] + this->m[ 5]*other.m[ 4] + this->m[ 6]*other.m[ 8] + this->m[ 7]*other.m[12],
  this->m[ 4]*other.m[ 1] + this->m[ 5]*other.m[ 5] + this->m[ 6]*other.m[ 9] + this->m[ 7]*other.m[13],
  this->m[ 4]*other.m[ 2] + this->m[ 5]*other.m[ 6] + this->m[ 6]*other.m[10] + this->m[ 7]*other.m[14],
  this->m[ 4]*other.m[ 3] + this->m[ 5]*other.m[ 7] + this->m[ 6]*other.m[11] + this->m[ 7]*other.m[15],
  this->m[ 8]*other.m[ 0] + this->m[ 9]*other.m[ 4] + this->m[10]*other.m[ 8] + this->m[11]*other.m[12],
  this->m[ 8]*other.m[ 1] + this->m[ 9]*other.m[ 5] + this->m[10]*other.m[ 9] + this->m[11]*other.m[13],
  this->m[ 8]*other.m[ 2] + this->m[ 9]*other.m[ 6] + this->m[10]*other.m[10] + this->m[11]*other.m[14],
  this->m[ 8]*other.m[ 3] + this->m[ 9]*other.m[ 7] + this->m[10]*other.m[11] + this->m[11]*other.m[15],
  this->m[12]*other.m[ 0] + this->m[13]*other.m[ 4] + this->m[14]*other.m[ 8] + this->m[15]*other.m[12],
  this->m[12]*other.m[ 1] + this->m[13]*other.m[ 5] + this->m[14]*other.m[ 9] + this->m[15]*other.m[13],
  this->m[12]*other.m[ 2] + this->m[13]*other.m[ 6] + this->m[14]*other.m[10] + this->m[15]*other.m[14],
  this->m[12]*other.m[ 3] + this->m[13]*other.m[ 7] + this->m[14]*other.m[11] + this->m[15]*other.m[15]
 };
 this->m[ 0] = temp[ 0];
 this->m[ 1] = temp[ 1];
 this->m[ 2] = temp[ 2];
 this->m[ 3] = temp[ 3];
 this->m[ 4] = temp[ 4];
 this->m[ 5] = temp[ 5];
 this->m[ 6] = temp[ 6];
 this->m[ 7] = temp[ 7];
 this->m[ 8] = temp[ 8];
 this->m[ 9] = temp[ 9];
 this->m[10] = temp[10];
 this->m[11] = temp[11];
 this->m[12] = temp[12];
 this->m[13] = temp[13];
 this->m[14] = temp[14];
 this->m[15] = temp[15];
 return *this;
}

#pragma endregion MATRIX4D_CLASS_DEFINITIONS

#pragma region MATRIX4D_OPERATORS

inline matrix4D operator +(const matrix4D& lhs, const matrix4D& rhs)
{
 matrix4D r;
 r.m[ 0] = lhs.m[ 0] + rhs.m[ 0];
 r.m[ 1] = lhs.m[ 1] + rhs.m[ 1];
 r.m[ 2] = lhs.m[ 2] + rhs.m[ 2];
 r.m[ 3] = lhs.m[ 3] + rhs.m[ 3];
 r.m[ 4] = lhs.m[ 4] + rhs.m[ 4];
 r.m[ 5] = lhs.m[ 5] + rhs.m[ 5];
 r.m[ 6] = lhs.m[ 6] + rhs.m[ 6];
 r.m[ 7] = lhs.m[ 7] + rhs.m[ 7];
 r.m[ 8] = lhs.m[ 8] + rhs.m[ 8];
 r.m[ 9] = lhs.m[ 9] + rhs.m[ 9];
 r.m[10] = lhs.m[10] + rhs.m[10];
 r.m[11] = lhs.m[11] + rhs.m[11];
 r.m[12] = lhs.m[12] + rhs.m[12];
 r.m[13] = lhs.m[13] + rhs.m[13];
 r.m[14] = lhs.m[14] + rhs.m[14];
 r.m[15] = lhs.m[15] + rhs.m[15];
 return r;
}

inline matrix4D operator -(const matrix4D& lhs, const matrix4D& rhs)
{
 matrix4D r;
 r.m[ 0] = lhs.m[ 0] - rhs.m[ 0];
 r.m[ 1] = lhs.m[ 1] - rhs.m[ 1];
 r.m[ 2] = lhs.m[ 2] - rhs.m[ 2];
 r.m[ 3] = lhs.m[ 3] - rhs.m[ 3];
 r.m[ 4] = lhs.m[ 4] - rhs.m[ 4];
 r.m[ 5] = lhs.m[ 5] - rhs.m[ 5];
 r.m[ 6] = lhs.m[ 6] - rhs.m[ 6];
 r.m[ 7] = lhs.m[ 7] - rhs.m[ 7];
 r.m[ 8] = lhs.m[ 8] - rhs.m[ 8];
 r.m[ 9] = lhs.m[ 9] - rhs.m[ 9];
 r.m[10] = lhs.m[10] - rhs.m[10];
 r.m[11] = lhs.m[11] - rhs.m[11];
 r.m[12] = lhs.m[12] - rhs.m[12];
 r.m[13] = lhs.m[13] - rhs.m[13];
 r.m[14] = lhs.m[14] - rhs.m[14];
 r.m[15] = lhs.m[15] - rhs.m[15];
 return r;
}

inline matrix4D operator *(const matrix4D& lhs, const matrix4D& rhs)
{
 matrix4D r;
 r.m[ 0] = lhs.m[ 0]*rhs.m[ 0] + lhs.m[ 1]*rhs.m[ 4] + lhs.m[ 2]*rhs.m[ 8] + lhs.m[ 3]*rhs.m[12];
 r.m[ 1] = lhs.m[ 0]*rhs.m[ 1] + lhs.m[ 1]*rhs.m[ 5] + lhs.m[ 2]*rhs.m[ 9] + lhs.m[ 3]*rhs.m[13];
 r.m[ 2] = lhs.m[ 0]*rhs.m[ 2] + lhs.m[ 1]*rhs.m[ 6] + lhs.m[ 2]*rhs.m[10] + lhs.m[ 3]*rhs.m[14];
 r.m[ 3] = lhs.m[ 0]*rhs.m[ 3] + lhs.m[ 1]*rhs.m[ 7] + lhs.m[ 2]*rhs.m[11] + lhs.m[ 3]*rhs.m[15];
 r.m[ 4] = lhs.m[ 4]*rhs.m[ 0] + lhs.m[ 5]*rhs.m[ 4] + lhs.m[ 6]*rhs.m[ 8] + lhs.m[ 7]*rhs.m[12];
 r.m[ 5] = lhs.m[ 4]*rhs.m[ 1] + lhs.m[ 5]*rhs.m[ 5] + lhs.m[ 6]*rhs.m[ 9] + lhs.m[ 7]*rhs.m[13];
 r.m[ 6] = lhs.m[ 4]*rhs.m[ 2] + lhs.m[ 5]*rhs.m[ 6] + lhs.m[ 6]*rhs.m[10] + lhs.m[ 7]*rhs.m[14];
 r.m[ 7] = lhs.m[ 4]*rhs.m[ 3] + lhs.m[ 5]*rhs.m[ 7] + lhs.m[ 6]*rhs.m[11] + lhs.m[ 7]*rhs.m[15];
 r.m[ 8] = lhs.m[ 8]*rhs.m[ 0] + lhs.m[ 9]*rhs.m[ 4] + lhs.m[10]*rhs.m[ 8] + lhs.m[11]*rhs.m[12];
 r.m[ 9] = lhs.m[ 8]*rhs.m[ 1] + lhs.m[ 9]*rhs.m[ 5] + lhs.m[10]*rhs.m[ 9] + lhs.m[11]*rhs.m[13];
 r.m[10] = lhs.m[ 8]*rhs.m[ 2] + lhs.m[ 9]*rhs.m[ 6] + lhs.m[10]*rhs.m[10] + lhs.m[11]*rhs.m[14];
 r.m[11] = lhs.m[ 8]*rhs.m[ 3] + lhs.m[ 9]*rhs.m[ 7] + lhs.m[10]*rhs.m[11] + lhs.m[11]*rhs.m[15];
 r.m[12] = lhs.m[12]*rhs.m[ 0] + lhs.m[13]*rhs.m[ 4] + lhs.m[14]*rhs.m[ 8] + lhs.m[15]*rhs.m[12];
 r.m[13] = lhs.m[12]*rhs.m[ 1] + lhs.m[13]*rhs.m[ 5] + lhs.m[14]*rhs.m[ 9] + lhs.m[15]*rhs.m[13];
 r.m[14] = lhs.m[12]*rhs.m[ 2] + lhs.m[13]*rhs.m[ 6] + lhs.m[14]*rhs.m[10] + lhs.m[15]*rhs.m[14];
 r.m[15] = lhs.m[12]*rhs.m[ 3] + lhs.m[13]*rhs.m[ 7] + lhs.m[14]*rhs.m[11] + lhs.m[15]*rhs.m[15];
 return r;
}

inline matrix4D operator *(const matrix4D& lhs, const real32& rhs)
{
 matrix4D r;
 r.m[ 0] = lhs.m[ 0] * rhs;
 r.m[ 1] = lhs.m[ 1] * rhs;
 r.m[ 2] = lhs.m[ 2] * rhs;
 r.m[ 3] = lhs.m[ 3] * rhs;
 r.m[ 4] = lhs.m[ 4] * rhs;
 r.m[ 5] = lhs.m[ 5] * rhs;
 r.m[ 6] = lhs.m[ 6] * rhs;
 r.m[ 7] = lhs.m[ 7] * rhs;
 r.m[ 8] = lhs.m[ 8] * rhs;
 r.m[ 9] = lhs.m[ 9] * rhs;
 r.m[10] = lhs.m[10] * rhs;
 r.m[11] = lhs.m[11] * rhs;
 r.m[12] = lhs.m[12] * rhs;
 r.m[13] = lhs.m[13] * rhs;
 r.m[14] = lhs.m[14] * rhs;
 r.m[15] = lhs.m[15] * rhs;
 return r;
}

inline matrix4D operator *(const real32& lhs, const matrix4D& rhs)
{
 matrix4D r;
 r.m[ 0] = rhs.m[ 0] * lhs;
 r.m[ 1] = rhs.m[ 1] * lhs;
 r.m[ 2] = rhs.m[ 2] * lhs;
 r.m[ 3] = rhs.m[ 3] * lhs;
 r.m[ 4] = rhs.m[ 4] * lhs;
 r.m[ 5] = rhs.m[ 5] * lhs;
 r.m[ 6] = rhs.m[ 6] * lhs;
 r.m[ 7] = rhs.m[ 7] * lhs;
 r.m[ 8] = rhs.m[ 8] * lhs;
 r.m[ 9] = rhs.m[ 9] * lhs;
 r.m[10] = rhs.m[10] * lhs;
 r.m[11] = rhs.m[11] * lhs;
 r.m[12] = rhs.m[12] * lhs;
 r.m[13] = rhs.m[13] * lhs;
 r.m[14] = rhs.m[14] * lhs;
 r.m[15] = rhs.m[15] * lhs;
 return r;
}

inline matrix4D operator /(const matrix4D& lhs, const real32& rhs)
{
 matrix4D r;
 r.m[ 0] = lhs.m[ 0] / rhs;
 r.m[ 1] = lhs.m[ 1] / rhs;
 r.m[ 2] = lhs.m[ 2] / rhs;
 r.m[ 3] = lhs.m[ 3] / rhs;
 r.m[ 4] = lhs.m[ 4] / rhs;
 r.m[ 5] = lhs.m[ 5] / rhs;
 r.m[ 6] = lhs.m[ 6] / rhs;
 r.m[ 7] = lhs.m[ 7] / rhs;
 r.m[ 8] = lhs.m[ 8] / rhs;
 r.m[ 9] = lhs.m[ 9] / rhs;
 r.m[10] = lhs.m[10] / rhs;
 r.m[11] = lhs.m[11] / rhs;
 r.m[12] = lhs.m[12] / rhs;
 r.m[13] = lhs.m[13] / rhs;
 r.m[14] = lhs.m[14] / rhs;
 r.m[15] = lhs.m[15] / rhs;
 return r;
}

inline matrix4D operator +(const matrix4D& rhs)
{
 return rhs;
}

inline matrix4D operator -(const matrix4D& rhs)
{
 matrix4D r;
 r.m[ 0] = -rhs.m[ 0];
 r.m[ 1] = -rhs.m[ 1];
 r.m[ 2] = -rhs.m[ 2];
 r.m[ 3] = -rhs.m[ 3];
 r.m[ 4] = -rhs.m[ 4];
 r.m[ 5] = -rhs.m[ 5];
 r.m[ 6] = -rhs.m[ 6];
 r.m[ 7] = -rhs.m[ 7];
 r.m[ 8] = -rhs.m[ 8];
 r.m[ 9] = -rhs.m[ 9];
 r.m[10] = -rhs.m[10];
 r.m[11] = -rhs.m[11];
 r.m[12] = -rhs.m[12];
 r.m[13] = -rhs.m[13];
 r.m[14] = -rhs.m[14];
 r.m[15] = -rhs.m[15];
 return r;
}

inline vector3D operator *(const matrix4D& lhs, const vector3D& rhs)
{
 return vector3D(
  lhs.m[0]*rhs[0] + lhs.m[1]*rhs[1] + lhs.m[ 2]*rhs[2] + lhs.m[ 3],
  lhs.m[4]*rhs[0] + lhs.m[5]*rhs[1] + lhs.m[ 6]*rhs[2] + lhs.m[ 7],
  lhs.m[8]*rhs[0] + lhs.m[9]*rhs[1] + lhs.m[10]*rhs[2] + lhs.m[11]
 );
}

#pragma endregion MATRIX4D_OPERATORS

#pragma region MATRIX4D_ARRAY_FUNCTIONS

inline void matrix4D_copy(real32* X, const real32* A)
{
 X[0x0] = A[0x0]; X[0x1] = A[0x1]; X[0x2] = A[0x2]; X[0x3] = A[0x3];
 X[0x4] = A[0x4]; X[0x5] = A[0x5]; X[0x6] = A[0x6]; X[0x7] = A[0x7];
 X[0x8] = A[0x8]; X[0x9] = A[0x9]; X[0xA] = A[0xA]; X[0xB] = A[0xB];
 X[0xC] = A[0xC]; X[0xD] = A[0xD]; X[0xE] = A[0xE]; X[0xF] = A[0xF];
}

inline void matrix4D_identity(real32* X)
{
 real32 one = 1.0f;
 real32 zer = 0.0f;
 X[0x0] = one; X[0x1] = zer; X[0x2] = zer; X[0x3] = zer;
 X[0x4] = zer; X[0x5] = one; X[0x6] = zer; X[0x7] = zer;
 X[0x8] = zer; X[0x9] = zer; X[0xA] = one; X[0xB] = zer;
 X[0xC] = zer; X[0xD] = zer; X[0xE] = zer; X[0xF] = one;
}

inline void matrix4D_transpose(real32* X, const real32* A)
{
 X[0x0] = A[0x0]; X[0x1] = A[0x4]; X[0x2] = A[0x8]; X[0x3] = A[0xC];
 X[0x4] = A[0x1]; X[0x5] = A[0x5]; X[0x6] = A[0x9]; X[0x7] = A[0xD];
 X[0x8] = A[0x2]; X[0x9] = A[0x6]; X[0xA] = A[0xA]; X[0xB] = A[0xE];
 X[0xC] = A[0x3]; X[0xD] = A[0x7]; X[0xE] = A[0xB]; X[0xF] = A[0xF];
}

inline void matrix4D_transpose(real32* A)
{
 real32 temp;
 temp = A[0x1]; A[0x1] = A[0x4]; A[0x4] = temp;
 temp = A[0x2]; A[0x2] = A[0x8]; A[0x8] = temp;
 temp = A[0x3]; A[0x3] = A[0xC]; A[0xC] = temp;
 temp = A[0x6]; A[0x6] = A[0x9]; A[0x9] = temp;
 temp = A[0x7]; A[0x7] = A[0xD]; A[0xD] = temp;
 temp = A[0xB]; A[0xB] = A[0xE]; A[0xE] = temp;
}

inline void matrix4D_add(real32* X, const real32* A, const real32* B)
{
 X[0x0] = (A[0x0] + B[0x0]); X[0x1] = (A[0x1] + B[0x1]); X[0x2] = (A[0x2] + B[0x2]); X[0x3] = (A[0x3] + B[0x3]);
 X[0x4] = (A[0x4] + B[0x4]); X[0x5] = (A[0x5] + B[0x5]); X[0x6] = (A[0x6] + B[0x6]); X[0x7] = (A[0x7] + B[0x7]);
 X[0x8] = (A[0x8] + B[0x8]); X[0x9] = (A[0x9] + B[0x9]); X[0xA] = (A[0xA] + B[0xA]); X[0xB] = (A[0xB] + B[0xB]);
 X[0xC] = (A[0xC] + B[0xC]); X[0xD] = (A[0xD] + B[0xD]); X[0xE] = (A[0xE] + B[0xE]); X[0xF] = (A[0xF] + B[0xF]);
}

inline void matrix4D_add(real32* A, const real32* B)
{
 A[0x0] += B[0x0]; A[0x1] += B[0x1]; A[0x2] += B[0x2]; A[0x3] += B[0x3];
 A[0x4] += B[0x4]; A[0x5] += B[0x5]; A[0x6] += B[0x6]; A[0x7] += B[0x7];
 A[0x8] += B[0x8]; A[0x9] += B[0x9]; A[0xA] += B[0xA]; A[0xB] += B[0xB];
 A[0xC] += B[0xC]; A[0xD] += B[0xD]; A[0xE] += B[0xE]; A[0xF] += B[0xF];
}

inline void matrix4D_sub(real32* X, const real32* A, const real32* B)
{
 X[0x0] = (A[0x0] - B[0x0]); X[0x1] = (A[0x1] - B[0x1]); X[0x2] = (A[0x2] - B[0x2]); X[0x3] = (A[0x3] - B[0x3]);
 X[0x4] = (A[0x4] - B[0x4]); X[0x5] = (A[0x5] - B[0x5]); X[0x6] = (A[0x6] - B[0x6]); X[0x7] = (A[0x7] - B[0x7]);
 X[0x8] = (A[0x8] - B[0x8]); X[0x9] = (A[0x9] - B[0x9]); X[0xA] = (A[0xA] - B[0xA]); X[0xB] = (A[0xB] - B[0xB]);
 X[0xC] = (A[0xC] - B[0xC]); X[0xD] = (A[0xD] - B[0xD]); X[0xE] = (A[0xE] - B[0xE]); X[0xF] = (A[0xF] - B[0xF]);
}

inline void matrix4D_sub(real32* A, const real32* B)
{
 A[0x0] -= B[0x0]; A[0x1] -= B[0x1]; A[0x2] -= B[0x2]; A[0x3] -= B[0x3];
 A[0x4] -= B[0x4]; A[0x5] -= B[0x5]; A[0x6] -= B[0x6]; A[0x7] -= B[0x7];
 A[0x8] -= B[0x8]; A[0x9] -= B[0x9]; A[0xA] -= B[0xA]; A[0xB] -= B[0xB];
 A[0xC] -= B[0xC]; A[0xD] -= B[0xD]; A[0xE] -= B[0xE]; A[0xF] -= B[0xF];
}

inline void matrix4D_mul(real32* X, const real32* A, const real32* B)
{
 X[0x0] = A[0x0]*B[0x0] + A[0x1]*B[0x4] + A[0x2]*B[0x8] + A[0x3]*B[0xC];
 X[0x1] = A[0x0]*B[0x1] + A[0x1]*B[0x5] + A[0x2]*B[0x9] + A[0x3]*B[0xD];
 X[0x2] = A[0x0]*B[0x2] + A[0x1]*B[0x6] + A[0x2]*B[0xA] + A[0x3]*B[0xE];
 X[0x3] = A[0x0]*B[0x3] + A[0x1]*B[0x7] + A[0x2]*B[0xB] + A[0x3]*B[0xF];
 X[0x4] = A[0x4]*B[0x0] + A[0x5]*B[0x4] + A[0x6]*B[0x8] + A[0x7]*B[0xC];
 X[0x5] = A[0x4]*B[0x1] + A[0x5]*B[0x5] + A[0x6]*B[0x9] + A[0x7]*B[0xD];
 X[0x6] = A[0x4]*B[0x2] + A[0x5]*B[0x6] + A[0x6]*B[0xA] + A[0x7]*B[0xE];
 X[0x7] = A[0x4]*B[0x3] + A[0x5]*B[0x7] + A[0x6]*B[0xB] + A[0x7]*B[0xF];
 X[0x8] = A[0x8]*B[0x0] + A[0x9]*B[0x4] + A[0xA]*B[0x8] + A[0xB]*B[0xC];
 X[0x9] = A[0x8]*B[0x1] + A[0x9]*B[0x5] + A[0xA]*B[0x9] + A[0xB]*B[0xD];
 X[0xA] = A[0x8]*B[0x2] + A[0x9]*B[0x6] + A[0xA]*B[0xA] + A[0xB]*B[0xE];
 X[0xB] = A[0x8]*B[0x3] + A[0x9]*B[0x7] + A[0xA]*B[0xB] + A[0xB]*B[0xF];
 X[0xC] = A[0xC]*B[0x0] + A[0xD]*B[0x4] + A[0xE]*B[0x8] + A[0xF]*B[0xC];
 X[0xD] = A[0xC]*B[0x1] + A[0xD]*B[0x5] + A[0xE]*B[0x9] + A[0xF]*B[0xD];
 X[0xE] = A[0xC]*B[0x2] + A[0xD]*B[0x6] + A[0xE]*B[0xA] + A[0xF]*B[0xE];
 X[0xF] = A[0xC]*B[0x3] + A[0xD]*B[0x7] + A[0xE]*B[0xB] + A[0xF]*B[0xF];
}

inline void matrix4D_mul(real32* A, const real32* B)
{
 real32 X[16] = {
  A[0x0]*B[0x0] + A[0x1]*B[0x4] + A[0x2]*B[0x8] + A[0x3]*B[0xC],
  A[0x0]*B[0x1] + A[0x1]*B[0x5] + A[0x2]*B[0x9] + A[0x3]*B[0xD],
  A[0x0]*B[0x2] + A[0x1]*B[0x6] + A[0x2]*B[0xA] + A[0x3]*B[0xE],
  A[0x0]*B[0x3] + A[0x1]*B[0x7] + A[0x2]*B[0xB] + A[0x3]*B[0xF],
  A[0x4]*B[0x0] + A[0x5]*B[0x4] + A[0x6]*B[0x8] + A[0x7]*B[0xC],
  A[0x4]*B[0x1] + A[0x5]*B[0x5] + A[0x6]*B[0x9] + A[0x7]*B[0xD],
  A[0x4]*B[0x2] + A[0x5]*B[0x6] + A[0x6]*B[0xA] + A[0x7]*B[0xE],
  A[0x4]*B[0x3] + A[0x5]*B[0x7] + A[0x6]*B[0xB] + A[0x7]*B[0xF],
  A[0x8]*B[0x0] + A[0x9]*B[0x4] + A[0xA]*B[0x8] + A[0xB]*B[0xC],
  A[0x8]*B[0x1] + A[0x9]*B[0x5] + A[0xA]*B[0x9] + A[0xB]*B[0xD],
  A[0x8]*B[0x2] + A[0x9]*B[0x6] + A[0xA]*B[0xA] + A[0xB]*B[0xE],
  A[0x8]*B[0x3] + A[0x9]*B[0x7] + A[0xA]*B[0xB] + A[0xB]*B[0xF],
  A[0xC]*B[0x0] + A[0xD]*B[0x4] + A[0xE]*B[0x8] + A[0xF]*B[0xC],
  A[0xC]*B[0x1] + A[0xD]*B[0x5] + A[0xE]*B[0x9] + A[0xF]*B[0xD],
  A[0xC]*B[0x2] + A[0xD]*B[0x6] + A[0xE]*B[0xA] + A[0xF]*B[0xE],
  A[0xC]*B[0x3] + A[0xD]*B[0x7] + A[0xE]*B[0xB] + A[0xF]*B[0xF]
 };
 A[0x0] = X[0x0];
 A[0x1] = X[0x1];
 A[0x2] = X[0x2];
 A[0x3] = X[0x3];
 A[0x4] = X[0x4];
 A[0x5] = X[0x5];
 A[0x6] = X[0x6];
 A[0x7] = X[0x7];
 A[0x8] = X[0x8];
 A[0x9] = X[0x9];
 A[0xA] = X[0xA];
 A[0xB] = X[0xB];
 A[0xC] = X[0xC];
 A[0xD] = X[0xD];
 A[0xE] = X[0xE];
 A[0xF] = X[0xF];
}

inline void matrix4D_vector3_mul(real32* X, const real32* A, const real32* B)
{
 X[0x0] = A[0x0]*B[0x0] + A[0x1]*B[0x1] + A[0x2]*B[0x2];
 X[0x1] = A[0x4]*B[0x0] + A[0x5]*B[0x1] + A[0x6]*B[0x2];
 X[0x2] = A[0x8]*B[0x0] + A[0x9]*B[0x1] + A[0xA]*B[0x2];
}

inline void matrix4D_vector4_mul(real32* X, const real32* A, const real32* B)
{
 X[0x0] = A[0x0]*B[0x0] + A[0x1]*B[0x1] + A[0x2]*B[0x2] + A[0x3]*B[0x3];
 X[0x1] = A[0x4]*B[0x0] + A[0x5]*B[0x1] + A[0x6]*B[0x2] + A[0x7]*B[0x3];
 X[0x2] = A[0x8]*B[0x0] + A[0x9]*B[0x1] + A[0xA]*B[0x2] + A[0xB]*B[0x3];
 X[0x3] = A[0xC]*B[0x0] + A[0xD]*B[0x1] + A[0xE]*B[0x2] + A[0xF]*B[0x3];
}

inline real32 inverse(real32* X, const real32* A)
{
 // cache 2x2 determinants
 real32 D[12] = {
  A[0xA]*A[0xF] - A[0xB]*A[0xE],
  A[0x9]*A[0xF] - A[0xB]*A[0xD],
  A[0x9]*A[0xE] - A[0xA]*A[0xD],
  A[0x8]*A[0xF] - A[0xB]*A[0xC],
  A[0x8]*A[0xE] - A[0xA]*A[0xC],
  A[0x8]*A[0xD] - A[0x9]*A[0xC],
  A[0x2]*A[0x7] - A[0x3]*A[0x6],
  A[0x1]*A[0x7] - A[0x3]*A[0x5],
  A[0x1]*A[0x6] - A[0x2]*A[0x5],
  A[0x0]*A[0x7] - A[0x3]*A[0x4],
  A[0x0]*A[0x6] - A[0x2]*A[0x4],
  A[0x0]*A[0x5] - A[0x1]*A[0x4]
 };

 // compute determinant
 real32 det = D[0x5]*D[0x6] - D[0x4]*D[0x7] + D[0x3]*D[0x8] + D[0x2]*D[0x9] - D[0x1]*D[0xA] + D[0x0]*D[0xB];
 if(det < 1.0e-7f) return det;

 // inv(M) = inv(det(M)) * transpose(C)
 real32 invdet = inv(det);
 X[0x0] = +(A[0x5]*D[0x0] - A[0x6]*D[0x1] + A[0x7]*D[0x2])*invdet;
 X[0x4] = -(A[0x4]*D[0x0] - A[0x6]*D[0x3] + A[0x7]*D[0x4])*invdet;
 X[0x8] = +(A[0x4]*D[0x1] - A[0x5]*D[0x3] + A[0x7]*D[0x5])*invdet;
 X[0xC] = -(A[0x4]*D[0x2] - A[0x5]*D[0x4] + A[0x6]*D[0x5])*invdet;
 X[0x1] = -(A[0x1]*D[0x0] - A[0x2]*D[0x1] + A[0x3]*D[0x2])*invdet;
 X[0x5] = +(A[0x0]*D[0x0] - A[0x2]*D[0x3] + A[0x3]*D[0x4])*invdet;
 X[0x9] = -(A[0x0]*D[0x1] - A[0x1]*D[0x3] + A[0x3]*D[0x5])*invdet;
 X[0xD] = +(A[0x0]*D[0x2] - A[0x1]*D[0x4] + A[0x2]*D[0x5])*invdet;
 X[0x2] = +(A[0xD]*D[0x6] - A[0xE]*D[0x7] + A[0xF]*D[0x8])*invdet;
 X[0x6] = -(A[0xC]*D[0x6] - A[0xE]*D[0x9] + A[0xF]*D[0xA])*invdet;
 X[0xA] = +(A[0xC]*D[0x7] - A[0xD]*D[0x9] + A[0xF]*D[0xB])*invdet;
 X[0xE] = -(A[0xC]*D[0x8] - A[0xD]*D[0xA] + A[0xE]*D[0xB])*invdet;
 X[0x3] = -(A[0x9]*D[0x6] - A[0xA]*D[0x7] + A[0xB]*D[0x8])*invdet;
 X[0x7] = +(A[0x8]*D[0x6] - A[0xA]*D[0x9] + A[0xB]*D[0xA])*invdet;
 X[0xB] = -(A[0x8]*D[0x7] - A[0x9]*D[0x9] + A[0xB]*D[0xB])*invdet;
 X[0xF] = +(A[0x8]*D[0x8] - A[0x9]*D[0xA] + A[0xA]*D[0xB])*invdet;

 // return determinant
 return det;
}

inline void matrix4D_rotate(real32* X, const real32* axis, real32 angle)
{
 // from Real-Time Rendering (Goldman equation)
 real32 cv = std::cos(angle);
 real32 sv = std::sin(angle);
 real32 dc = 1.0f - cv;
 real32 p012[2] = { dc*(axis[0]*axis[1]), sv*axis[2] };
 real32 p021[2] = { dc*(axis[0]*axis[2]), sv*axis[1] };
 real32 p120[2] = { dc*(axis[1]*axis[2]), sv*axis[0] };

 // row 0
 X[0x0] = cv + dc*(axis[0]*axis[0]);
 X[0x1] = p012[0] - p012[1];
 X[0x2] = p021[0] + p021[1];
 X[0x3] = 0.0f;

 // row 1
 X[0x4] = p012[0] + p012[1];
 X[0x5] = cv + dc*(axis[1]*axis[1]);
 X[0x6] = p120[0] - p120[1];
 X[0x7] = 0.0f;

 // row 2
 X[0x8] = p021[0] - p021[1];
 X[0x9] = p120[0] + p120[1];
 X[0xA] = cv + dc*(axis[2]*axis[2]);
 X[0xB] = 0.0f;

 // row 3
 X[0xC] = 0.0f;
 X[0xD] = 0.0f;
 X[0xE] = 0.0f;
 X[0xF] = 1.0f;
}

#pragma endregion MATRIX4D_ARRAY_FUNCTIONS

#endif
