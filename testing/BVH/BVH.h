#ifndef __CS489_BVH_H
#define __CS489_BVH_H

template<class T>
struct minmax_AABB {
 #ifndef __USE_SSE__
 vector3D<T> min_v[3];
 vector3D<T> max_v[3];
 #else
 alignas(16) vector4D<T> min_v[3];
 alignas(16) vector4D<T> max_v[3];
 #endif
};

template<class T>
class BVH {
};

#endif
