#ifndef __CS489_RAY_H
#define __CS489_RAY_H

#include "math.h"
#include "vector3.h"

#pragma region INTERSECTION_FUNCTIONS

/** \brief   Ray-Plane Intersection Distance.
 *  \details Computes distance from a point (O) and a plane (P) along a ray vector (V). If the
 *           ray and plane do not intersect, the return value is positive infinity.
 *  \author  Steven F. Emory
 *  \date    02/05/2018
 */
inline real32 ray3D_plane_intersect_distance(real32* X, const real32* O, const real32* V, const real32* P)
{
 real32 denom = vector3D_scalar_product(P, V);
 if(std::abs(denom) < epsilon()) return positive_infinity();
 real32 numer = vector3D_scalar_product(P, O) + P[3];
 real32 distance = numer/denom;
 X[0] = O[0] + V[0]*distance;
 X[1] = O[1] + V[1]*distance;
 X[2] = O[2] + V[2]*distance;
 return -distance;
}

#pragma endregion INTERSECTION_FUNCTIONS

#endif
