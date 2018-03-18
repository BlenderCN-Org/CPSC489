#ifndef __CS489_SPHERE3_H
#define __CS489_SPHERE3_H

#include "math.h"

struct sphere3D {
 real32 center[3];
 real32 radius;
};

inline void load_unit(sphere3D& b)
{
 b.center[0] = 0.0f;
 b.center[1] = 0.0f;
 b.center[2] = 0.0f;
 b.radius    = 1.0f;
}

inline void scale(sphere3D& b, real32 value)
{
 b.radius *= value;
}

inline void translate(sphere3D& b, const real32* v)
{
 b.center[0] += v[0];
 b.center[1] += v[1];
 b.center[2] += v[2];
}

#endif
