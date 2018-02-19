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

#pragma region STRUCTURES

template<class T>
struct vector2D {
 T v[2];
};

template<class T>
struct vector3D {
 T v[3];
};

template<class T>
struct vector4D {
 T v[4];
};

template<class T>
struct matrix4D {
 T v[16];
};

#pragma endregion STRUCTURES

#endif
