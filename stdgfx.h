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

#endif
