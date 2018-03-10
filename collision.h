#ifndef __CS_COLLISION_H
#define __CS_COLLISION_H

// AABB INTERSECT WITH X
#pragma region AABB_INTERSECTION

inline bool AABB_intersect(const AABB_halfdim& aabb, const float* v)
{
 // x-axis test
 if(v[0] < (aabb.center[0] - aabb.widths[0])) return false;
 if(v[0] > (aabb.center[0] + aabb.widths[0])) return false;

 // y-axis test
 if(v[1] < (aabb.center[1] - aabb.widths[1])) return false;
 if(v[1] > (aabb.center[1] + aabb.widths[1])) return false;

 // z-axis test
 if(v[2] < (aabb.center[2] - aabb.widths[2])) return false;
 if(v[2] > (aabb.center[2] + aabb.widths[2])) return false;

 return true;
}

inline bool AABB_intersect(const AABB_halfdim& b1, const AABB_halfdim& b2)
{
 // x-axis test
 if((b2.center[0] + b2.widths[0]) < (b1.center[0] - b1.widths[0])) return false;
 if((b1.center[0] + b1.widths[0]) < (b2.center[0] - b2.widths[0])) return false;

 // y-axis test
 if((b2.center[1] + b2.widths[1]) < (b1.center[1] - b1.widths[1])) return false;
 if((b1.center[1] + b1.widths[1]) < (b2.center[1] - b2.widths[1])) return false;

 // z-axis test
 if((b2.center[2] + b2.widths[2]) < (b1.center[2] - b1.widths[2])) return false;
 if((b1.center[2] + b1.widths[2]) < (b2.center[2] - b2.widths[2])) return false;
}

#pragma endregion AABB_INTERSECTION

// OBB INTERSECT WITH X
#pragma region OBB_INTERSECTION

inline bool OBB_intersect(const OBB& obb, const float* v)
{
 // move OBB coordinate system to origin
 // p[0] = v[0] - obb.center[0],
 // p[1] = v[1] - obb.center[1],
 // p[2] = v[2] - obb.center[2]

 // this is OBB matrix
 // obb.x[0] obb.x[1] obb.x[2]
 // obb.y[0] obb.y[1] obb.y[2]
 // obb.z[0] obb.z[1] obb.z[2]

 // this is OBB matrix inverse (transpose)
 // obb.x[0] obb.y[0] obb.z[0]
 // obb.x[1] obb.y[1] obb.z[1]
 // obb.x[2] obb.y[2] obb.z[2]

 // to align OBB axes to world space axes, multiply OBB coordinate system by its matrix inverse
 // the result is an AABB test
 // | obb.x[0] obb.y[0] obb.z[0] |   | p[0] |   | obb.x[0]*p[0] + obb.y[0]*p[1] + obb.z[0]*p[2] |
 // | obb.x[1] obb.y[1] obb.z[1] | * | p[1] | = | obb.x[1]*p[0] + obb.y[1]*p[1] + obb.z[1]*p[2] |
 // | obb.x[2] obb.y[2] obb.z[2] |   | p[2] |   | obb.x[2]*p[0] + obb.y[2]*p[1] + obb.z[2]*p[2] |

 // move OBB to origin
 float p[3] = {
  v[0] - obb.center[0],
  v[1] - obb.center[1],
  v[2] - obb.center[2]
 };

 // point in world space, OBB now AABB 
 float dots[3] = {
  obb.x[0]*p[0] + obb.y[0]*p[1] + obb.z[0]*p[2],
  obb.x[1]*p[0] + obb.y[1]*p[1] + obb.z[1]*p[2],
  obb.x[2]*p[0] + obb.y[2]*p[1] + obb.z[2]*p[2] 
 };

 // x-axis test
 if(p[0] < -obb.widths[0]) return false;
 if(p[0] > +obb.widths[0]) return false;

 // y-axis test
 if(p[1] < -obb.widths[1]) return false;
 if(p[1] > +obb.widths[1]) return false;

 // z-axis test
 if(p[2] < -obb.widths[2]) return false;
 if(p[2] > +obb.widths[2]) return false;

 return true;
}

#pragma endregion OBB_INTERSECTION

#endif
