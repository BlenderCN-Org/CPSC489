#ifndef __CPSC489_STDGFX_H
#define __CPSC489_STDGFX_H

#pragma region AXIS_FUNCTIONS

inline void POSITIVE_X_AXIS(float* ptr) { ptr[0] = +1.0f; ptr[1] =  0.0f; ptr[2] =  0.0f; }
inline void NEGATIVE_X_AXIS(float* ptr) { ptr[0] = -1.0f; ptr[1] =  0.0f; ptr[2] =  0.0f; }
inline void POSITIVE_Y_AXIS(float* ptr) { ptr[0] =  0.0f; ptr[1] = +1.0f; ptr[2] =  0.0f; }
inline void NEGATIVE_Y_AXIS(float* ptr) { ptr[0] =  0.0f; ptr[1] = -1.0f; ptr[2] =  0.0f; }
inline void POSITIVE_Z_AXIS(float* ptr) { ptr[0] =  0.0f; ptr[1] =  0.0f; ptr[2] = +1.0f; }
inline void NEGATIVE_Z_AXIS(float* ptr) { ptr[0] =  0.0f; ptr[1] =  0.0f; ptr[2] = -1.0f; }

#pragma endregion AXIS_FUNCTIONS

#pragma region SPECIAL_FUNCTIONS

inline real32 inv(real32 x) { return 1.0f/x; }

#pragma endregion SPECIAL_FUNCTIONS

#pragma region SPECIAL_NUMBERS

inline real32 positive_infinity(void)
{
 // 0-11111111-00000000000000000000000
 uint32 x = 0x7F800000u;
 return *reinterpret_cast<real32*>(&x);
}

inline real32 negative_infinity(void)
{
 // 1-11111111-00000000000000000000000
 uint32 x = 0xFF800000u;
 return *reinterpret_cast<real32*>(&x);
}

#pragma endregion SPECIAL_NUMBERS

#pragma region TRIGNOMETRY

// Trignometric Constants
inline real32 pi(void)           { uint32 x = 0x40490FDBul; return *reinterpret_cast<real32*>(&x); } // oeis.org code:A000796
inline real32 pi_times_2(void)   { return 6.28318530718f; } // oeis.org code:A019692
inline real32 pi_over_2(void)    { return 1.57079632679f; } // oeis.org code:A019669
inline real32 pi_over_3(void)    { return 1.04719755120f; } // oeis.org code:A019670
inline real32 pi_over_4(void)    { return 0.78539816340f; } // oeis.org code:A003881
inline real32 pi_over_180(void)  { return 0.01745329252f; } // oeis.org code:A019685
inline real32 pi_under_180(void) { return 57.2957795131f; } // oeis.org code:A072097

// Trignometric Conversions
inline real32 radians(real32 d) { return d*pi_over_180(); }
inline real32 degrees(real32 r) { return r*pi_under_180(); }

#pragma endregion TRIGNOMETRY

#pragma region VECTOR_FUNCTIONS

inline real32 dot_product(const real32* A, const real32* B)
{
 return A[0]*B[0] + A[1]*B[1] + A[2]*B[2];
}

inline void cross_product(real32* X, const real32* A, const real32* B)
{
 X[0] = A[1]*B[2] - A[2]*B[1];
 X[1] = A[2]*B[0] - A[0]*B[2];
 X[2] = A[0]*B[1] - A[1]*B[0];
}

inline real32 scalar_triple_product(const real32* A, const real32* B, const real32* C)
{
 return ((B[1]*C[2] - B[2]*C[1])*A[0] + (B[2]*C[0] - B[0]*C[2])*A[1] + (B[0]*C[1] - B[1]*C[0])*A[2]);
}

inline real32 vector3_squared_norm(const real32* A)
{
 return (A[0]*A[0]) + (A[1]*A[1]) + (A[2]*A[2]);
}

inline real32 vector3_norm(const real32* A)
{
 return std::sqrt(vector3_squared_norm(A));
}

inline void vector3_normalize(real32* X)
{
 real32 scalar = inv(vector3_norm(X));
 X[0] *= scalar;
 X[1] *= scalar;
 X[2] *= scalar;
}

#pragma endregion VECTOR_FUNCTIONS

#pragma region MATRIX_FUNCTIONS

#pragma endregion MATRIX_FUNCTIONS

#pragma region RAY_FUNCTIONS

/** \brief   Ray-Plane Intersection Distance.
 *  \details Computes distance from a point (O) and a plane (P) along a ray vector (V). If the
 *           ray and plane do not intersect, the return value is positive infinity.
 *  \author  Steven F. Emory
 *  \date    02/05/2018
 */
inline real32 ray3D_plane_intersect_distance(real32* X, const real32* O, const real32* V, const real32* P)
{
 real32 denom = dot_product(P, V);
 if(std::abs(denom) == 0.0f) return positive_infinity();
 real32 numer = dot_product(P, O) + P[3];
 real32 distance = numer/denom;
 X[0] = O[0] + V[0]*distance;
 X[1] = O[1] + V[1]*distance;
 X[2] = O[2] + V[2]*distance;
 return -distance;
}

#pragma endregion RAY_FUNCTIONS

#endif
