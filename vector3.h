#ifndef __CPSC489_VECTOR3_H
#define __CPSC489_VECTOR3_H

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

#endif
