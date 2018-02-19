#ifndef __CPSC489_VECTOR3_H
#define __CPSC489_VECTOR3_H

#pragma region VECTOR3D_CLASS_DEFINITIONS

// c_svector3D sv;
// sv.v[0] = 1;
// sv.v[1] = 2;
// sv.v[2] = 3;
// auto x = reinterpret_cast<real32(&)[3]>(sv)[0];
// auto y = reinterpret_cast<real32(&)[3]>(sv)[1];
// auto z = reinterpret_cast<real32(&)[3]>(sv)[2];
// cout << "x = " << x << endl;
// cout << "y = " << y << endl;
// cout << "z = " << z << endl;

// c_svector3D av[3];
// av[0].v[0] = 1; av[1].v[0] = 4; av[2].v[0] = 7;
// av[0].v[1] = 2; av[1].v[1] = 5; av[2].v[1] = 8;
// av[0].v[2] = 3; av[1].v[2] = 6; av[2].v[2] = 9;
// auto x1 = reinterpret_cast<real32*>(av)[1];
// auto x2 = reinterpret_cast<real32*>(av)[4];
// auto x3 = reinterpret_cast<real32*>(av)[7];
// cout << "x1 = " << x1 << endl;
// cout << "x2 = " << x2 << endl;
// cout << "x3 = " << x3 << endl;

class vector3D;
typedef struct _c_svector3D { real32 v[3]; } c_svector3D;
class vector3D : public c_svector3D {
 public :
  typedef real32 value_type;
 public :
  vector3D() {
  }
  vector3D(real32 x, real32 y, real32 z) {
   this->v[0] = x;
   this->v[1] = y;
   this->v[2] = z;
  }
  explicit vector3D(const real32* v) {
   this->v[0] = v[0];
   this->v[1] = v[1];
   this->v[2] = v[2];
  }
  vector3D(const vector3D& other) {
   this->v[0] = other.v[0];
   this->v[1] = other.v[1];
   this->v[2] = other.v[2];
  }
 public :
  real32 x(void)const { return this->v[0]; }
  real32 y(void)const { return this->v[1]; }
  real32 z(void)const { return this->v[2]; }
 public :
  void x(real32 value) { this->v[0] = value; }
  void y(real32 value) { this->v[1] = value; }
  void z(real32 value) { this->v[2] = value; }
  void reset(real32 x, real32 y, real32 z) {
   this->v[0] = x;
   this->v[1] = y;
   this->v[2] = z;
  }
  void reset(const real32* v) {
   this->v[0] = v[0];
   this->v[1] = v[1];
   this->v[2] = v[2];
  }
 public :
  real32& operator [](int index) { return this->v[index]; }
  const real32& operator [](int index)const { return this->v[index]; }
 public :
  vector3D& operator =(const vector3D& other) {
   this->v[0] = other.v[0];
   this->v[1] = other.v[1];
   this->v[2] = other.v[2];
   return *this;
  }
 public :
  vector3D& operator +=(const vector3D& other) {
   this->v[0] += other.v[0];
   this->v[1] += other.v[1];
   this->v[2] += other.v[2];
   return *this;
  }
  vector3D& operator -=(const vector3D& other) {
   this->v[0] -= other.v[0];
   this->v[1] -= other.v[1];
   this->v[2] -= other.v[2];
   return *this;
  }
  vector3D& operator *=(real32 scalar) {
   this->v[0] *= scalar;
   this->v[1] *= scalar;
   this->v[2] *= scalar;
   return *this;
  }
  vector3D& operator /=(real32 scalar) {
   this->v[0] /= scalar;
   this->v[1] /= scalar;
   this->v[2] /= scalar;
   return *this;
  }
};

#pragma endregion VECTOR3D_CLASS_DEFINITIONS

#pragma region VECTOR3D_NONMEMBER_OPERATORS

inline vector3D operator +(const vector3D& lhs, const vector3D& rhs)
{
 return vector3D(
  lhs.v[0] + rhs.v[0],
  lhs.v[1] + rhs.v[1],
  lhs.v[2] + rhs.v[2]
 );
}

inline vector3D operator -(const vector3D& lhs, const vector3D& rhs)
{
 return vector3D(
  lhs.v[0] - rhs.v[0],
  lhs.v[1] - rhs.v[1],
  lhs.v[2] - rhs.v[2]
 );
}

inline vector3D operator *(const vector3D& lhs, const real32& rhs)
{
 return vector3D(
  lhs.v[0] * rhs,
  lhs.v[1] * rhs,
  lhs.v[2] * rhs
 );
}

inline vector3D operator *(const real32& lhs, const vector3D& rhs)
{
 return vector3D(
  rhs.v[0] * lhs,
  rhs.v[1] * lhs,
  rhs.v[2] * lhs
 );
}

inline vector3D operator /(const vector3D& lhs, const real32& rhs)
{
 return vector3D(
  lhs.v[0] / rhs,
  lhs.v[1] / rhs,
  lhs.v[2] / rhs
 );
}

inline vector3D operator +(const vector3D& rhs)
{
 return rhs;
}

inline vector3D operator -(const vector3D& rhs)
{
 return vector3D(-rhs.v[0], -rhs.v[1], -rhs.v[2]);
}

inline bool operator ==(const vector3D& lhs, const vector3D& rhs)
{
 return ((lhs.v[0] == rhs.v[0]) &&
         (lhs.v[1] == rhs.v[1]) &&
         (lhs.v[2] == rhs.v[2]));
}

inline bool operator !=(const vector3D& lhs, const vector3D& rhs)
{
 return !(lhs == rhs);
}

template<class charT, class traits>
std::basic_ostream<charT, traits>& operator <<(std::basic_ostream<charT, traits>& os, const vector3D& rhs)
{
 // copy
 std::basic_ostringstream<charT, traits> oss;
 oss.flags(os.flags());
 oss.imbue(os.getloc());
 oss.precision(os.precision());
 oss << '<' << rhs.v[0] << ',' << rhs.v[1] << ',' << rhs.v[2] << '>';

 // output to stream
 std::basic_string<charT, traits> str = oss.str();
 return (os << str.c_str());
}

#pragma endregion VECTOR3D_NONMEMBER_OPERATORS

#pragma region VECTOR3D_NONMEMBER_FUNCTIONS

inline real32 squared_norm(const vector3D& v)
{
 return (v.v[0]*v.v[0]) + (v.v[1]*v.v[1]) + (v.v[2]*v.v[2]);
}

inline real32 norm(const vector3D& v)
{
 return sqrt(squared_norm(v));
}

inline vector3D unit(const vector3D& v)
{
 real32 scalar = inv(norm(v));
 return vector3D(v.v[0]*scalar, v.v[1]*scalar, v.v[2]*scalar);
}

inline void normalize(vector3D& v)
{
 real32 scalar = inv(norm(v));
 v.v[0] *= scalar;
 v.v[1] *= scalar;
 v.v[2] *= scalar;
}

inline real32 length(const vector3D& v)
{
 return sqrt(squared_norm(v));
}

inline real32 scalar_product(const vector3D& v1, const vector3D& v2)
{
 return (v1.v[0]*v2.v[0]) + (v1.v[1]*v2.v[1]) + (v1.v[2]*v2.v[2]);
}

inline vector3D vector_product(const vector3D& v1, const vector3D& v2)
{
 return vector3D(
  v1.v[1]*v2.v[2] - v1.v[2]*v2.v[1],
  v1.v[2]*v2.v[0] - v1.v[0]*v2.v[2],
  v1.v[0]*v2.v[1] - v1.v[1]*v2.v[0]
 );
}

inline real32 scalar_triple_product(const vector3D& v0, const vector3D& v1, const vector3D& v2)
{
 return (
  (v1.v[1]*v2.v[2] - v1.v[2]*v2.v[1])*v0.v[0] +
  (v1.v[2]*v2.v[0] - v1.v[0]*v2.v[2])*v0.v[1] +
  (v1.v[0]*v2.v[1] - v1.v[1]*v2.v[0])*v0.v[2]
 );
}

#pragma endregion VECTOR3D_NONMEMBER_FUNCTIONS

#pragma region VECTOR3D_ARRAY_FUNCTIONS

template<class T, typename std::enable_if<std::is_floating_point<T>::value, T*>::type = nullptr>
inline T vector3D_squared_norm(const T* A)
{
 return (A[0]*A[0]) + (A[1]*A[1]) + (A[2]*A[2]);
}

template<class T, typename std::enable_if<std::is_floating_point<T>::value, T*>::type = nullptr>
inline T vector3D_norm(const T* A)
{
 return std::sqrt(vector3D_squared_norm(A));
}

template<class T, typename std::enable_if<std::is_floating_point<T>::value, T*>::type = nullptr>
inline void vector3D_unit(T* X, const T* A)
{
 T scalar = inv(vector3D_norm(X));
 X[0] *= scalar;
 X[1] *= scalar;
 X[2] *= scalar;
}

template<class T, typename std::enable_if<std::is_floating_point<T>::value, T*>::type = nullptr>
inline void vector3D_normalize(T* X)
{
 T scalar = inv(vector3D_norm(X));
 X[0] *= scalar;
 X[1] *= scalar;
 X[2] *= scalar;
}

template<class T, typename std::enable_if<std::is_floating_point<T>::value, T*>::type = nullptr>
inline T vector3D_length(const T* A)
{
 return std::sqrt(vector3D_squared_norm(A));
}

template<class T, typename std::enable_if<std::is_floating_point<T>::value, T*>::type = nullptr>
inline real32 vector3D_scalar_product(const T* A, const T* B)
{
 return A[0]*B[0] + A[1]*B[1] + A[2]*B[2];
}

template<class T, typename std::enable_if<std::is_floating_point<T>::value, T*>::type = nullptr>
inline void vector3D_vector_product(T* X, const T* A, const T* B)
{
 X[0] = A[1]*B[2] - A[2]*B[1];
 X[1] = A[2]*B[0] - A[0]*B[2];
 X[2] = A[0]*B[1] - A[1]*B[0];
}

template<class T, typename std::enable_if<std::is_floating_point<T>::value, T*>::type = nullptr>
inline T vector3D_scalar_triple_product(const T* A, const T* B, const T* C)
{
 return ((B[1]*C[2] - B[2]*C[1])*A[0] + (B[2]*C[0] - B[0]*C[2])*A[1] + (B[0]*C[1] - B[1]*C[0])*A[2]);
}

#pragma endregion VECTOR3D_ARRAY_FUNCTIONS

#endif
