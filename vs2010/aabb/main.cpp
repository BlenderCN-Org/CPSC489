#include<iostream>

struct AABB_halfdim {
 float center[4];
 float widths[4];
};

struct OBB {
 float center[4];
 float x[4];
 float y[4];
 float z[4];
 float widths[4];
};

struct sphere {
 float center[4];
 float radius;
};

inline float radians(float d) { return d*0.01745329252f; }
inline float degrees(float r) { return r*57.2957795131f; }

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

inline bool AABB_intersect(const AABB_halfdim& aabb, const sphere& s)
{
 // x-axis test
 // if((s.center[0] + s.radius) < (aabb.center[0] - aabb.widths[0])) return false;
 // if((s.center[0] - s.radius) > (aabb.center[0] + aabb.widths[0])) return false;
 float d1 = s.center[0] - aabb.center[0];
 float d2 = s.radius + aabb.widths[0];
 if(d1 < -d2) return false;
 if(d1 > +d2) return false;

 // y-axis test
 if((s.center[1]) < (aabb.center[1] - aabb.widths[1])) return false;
 if((s.center[1]) > (aabb.center[1] + aabb.widths[1])) return false;

 // z-axis test
 if((s.center[2]) < (aabb.center[2] - aabb.widths[2])) return false;
 if((s.center[2]) > (aabb.center[2] + aabb.widths[2])) return false;

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

inline bool OBB_intersect(const OBB& obb, const float* v)
{
 // move OBB to origin
 float p[3] = {
  v[0] - obb.center[0],
  v[1] - obb.center[1],
  v[2] - obb.center[2]
 };

 // this is OBB matrix
 // obb.x[0] obb.x[1] obb.x[2]
 // obb.y[0] obb.y[1] obb.y[2]
 // obb.z[0] obb.z[1] obb.z[2]

 // this is OBB matrix inverse (transpose)
 // obb.x[0] obb.y[0] obb.z[0]
 // obb.x[1] obb.y[1] obb.z[1]
 // obb.x[2] obb.y[2] obb.z[2]

 // to send point in OBB space to world space, multiply by OBB matrix inverse
 // | obb.x[0] obb.y[0] obb.z[0] |   | p[0] |   | obb.x[0]*p[0] + obb.y[0]*p[1] + obb.z[0]*p[2] |
 // | obb.x[1] obb.y[1] obb.z[1] | * | p[1] | = | obb.x[1]*p[0] + obb.y[1]*p[1] + obb.z[1]*p[2] |
 // | obb.x[2] obb.y[2] obb.z[2] |   | p[2] |   | obb.x[2]*p[0] + obb.y[2]*p[1] + obb.z[2]*p[2] |

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

inline void OBB_vertices(const OBB& obb, float b[][3])
{
 // +, +, +
 b[0][0] = obb.center[0] + (obb.widths[0]*obb.x[0] + obb.widths[1]*obb.x[1] + obb.widths[2]*obb.x[2]);
 b[0][1] = obb.center[1] + (obb.widths[0]*obb.y[0] + obb.widths[1]*obb.y[1] + obb.widths[2]*obb.y[2]);
 b[0][2] = obb.center[2] + (obb.widths[0]*obb.z[0] + obb.widths[1]*obb.z[1] + obb.widths[2]*obb.z[2]);

 // +, +, -
 b[1][0] = obb.center[0] + (obb.widths[0]*obb.x[0] + obb.widths[1]*obb.x[1] - obb.widths[2]*obb.x[2]);
 b[1][1] = obb.center[1] + (obb.widths[0]*obb.y[0] + obb.widths[1]*obb.y[1] - obb.widths[2]*obb.y[2]);
 b[1][2] = obb.center[2] + (obb.widths[0]*obb.z[0] + obb.widths[1]*obb.z[1] - obb.widths[2]*obb.z[2]);

 // +, -, +
 b[2][0] = obb.center[0] + (obb.widths[0]*obb.x[0] - obb.widths[1]*obb.x[1] + obb.widths[2]*obb.x[2]);
 b[2][1] = obb.center[1] + (obb.widths[0]*obb.y[0] - obb.widths[1]*obb.y[1] + obb.widths[2]*obb.y[2]);
 b[2][2] = obb.center[2] + (obb.widths[0]*obb.z[0] - obb.widths[1]*obb.z[1] + obb.widths[2]*obb.z[2]);

 // +, -, -
 b[3][0] = obb.center[0] + (obb.widths[0]*obb.x[0] - obb.widths[1]*obb.x[1] - obb.widths[2]*obb.x[2]);
 b[3][1] = obb.center[1] + (obb.widths[0]*obb.y[0] - obb.widths[1]*obb.y[1] - obb.widths[2]*obb.y[2]);
 b[3][2] = obb.center[2] + (obb.widths[0]*obb.z[0] - obb.widths[1]*obb.z[1] - obb.widths[2]*obb.z[2]);

 // -, +, +
 b[4][0] = obb.center[0] + (-obb.widths[0]*obb.x[0] + obb.widths[1]*obb.x[1] + obb.widths[2]*obb.x[2]);
 b[4][1] = obb.center[1] + (-obb.widths[0]*obb.y[0] + obb.widths[1]*obb.y[1] + obb.widths[2]*obb.y[2]);
 b[4][2] = obb.center[2] + (-obb.widths[0]*obb.z[0] + obb.widths[1]*obb.z[1] + obb.widths[2]*obb.z[2]);

 // -, +, -
 b[5][0] = obb.center[0] + (-obb.widths[0]*obb.x[0] + obb.widths[1]*obb.x[1] - obb.widths[2]*obb.x[2]);
 b[5][1] = obb.center[1] + (-obb.widths[0]*obb.y[0] + obb.widths[1]*obb.y[1] - obb.widths[2]*obb.y[2]);
 b[5][2] = obb.center[2] + (-obb.widths[0]*obb.z[0] + obb.widths[1]*obb.z[1] - obb.widths[2]*obb.z[2]);

 // -, -, +
 b[6][0] = obb.center[0] + (-obb.widths[0]*obb.x[0] - obb.widths[1]*obb.x[1] + obb.widths[2]*obb.x[2]);
 b[6][1] = obb.center[1] + (-obb.widths[0]*obb.y[0] - obb.widths[1]*obb.y[1] + obb.widths[2]*obb.y[2]);
 b[6][2] = obb.center[2] + (-obb.widths[0]*obb.z[0] - obb.widths[1]*obb.z[1] + obb.widths[2]*obb.z[2]);

 // -, -, -
 b[7][0] = obb.center[0] + (-obb.widths[0]*obb.x[0] - obb.widths[1]*obb.x[1] - obb.widths[2]*obb.x[2]);
 b[7][1] = obb.center[1] + (-obb.widths[0]*obb.y[0] - obb.widths[1]*obb.y[1] - obb.widths[2]*obb.y[2]);
 b[7][2] = obb.center[2] + (-obb.widths[0]*obb.z[0] - obb.widths[1]*obb.z[1] - obb.widths[2]*obb.z[2]);
}

void matrix3D_mul(float* X, const float* A, const float* B)
{
 float T[9] = {
  A[0]*B[0] + A[1]*B[3] + A[2]*B[6],
  A[0]*B[1] + A[1]*B[4] + A[2]*B[7],
  A[0]*B[2] + A[1]*B[5] + A[2]*B[8],
  A[3]*B[0] + A[4]*B[3] + A[5]*B[6],
  A[3]*B[1] + A[4]*B[4] + A[5]*B[7],
  A[3]*B[2] + A[4]*B[5] + A[5]*B[8],
  A[6]*B[0] + A[7]*B[3] + A[8]*B[6],
  A[6]*B[1] + A[7]*B[4] + A[8]*B[7],
  A[6]*B[2] + A[7]*B[5] + A[8]*B[8]
 };
 X[0] = T[0];
 X[1] = T[1];
 X[2] = T[2];
 X[3] = T[3];
 X[4] = T[4];
 X[5] = T[5];
 X[6] = T[6];
 X[7] = T[7];
 X[8] = T[8];
}

void matrix3D_mul_vector3D(float* X, const float* A, const float* B)
{
 float T[3] = {
  A[0]*B[0] + A[1]*B[0] + A[2]*B[0],
  A[3]*B[1] + A[4]*B[1] + A[5]*B[1],
  A[6]*B[2] + A[7]*B[2] + A[8]*B[2]
 };
 X[0] = T[0];
 X[1] = T[1];
 X[2] = T[2];
}

inline void matrix3D_rotate_XYZ(float* m, float r1, float r2, float r3)
{
 // cached cosines
 float c1 = std::cos(r1);
 float c2 = std::cos(r2);
 float c3 = std::cos(r3);

 // cached sines
 float s1 = std::sin(r1);
 float s2 = std::sin(r2);
 float s3 = std::sin(r3);

 // composite values
 float c3_s2 = c3 * s2;
 float s2_s3 = s2 * s3;

 // set matrix
 m[0] =  (c2 * c3);
 m[1] = -(c2 * s3);
 m[2] =  s2;
 m[3] =  (c1 * s3) + (s1 * c3_s2);
 m[4] =  (c1 * c3) - (s1 * s2_s3);
 m[5] = -(c2 * s1);
 m[6] =  (s1 * s3) - (c1 * c3_s2);
 m[7] =  (c3 * s1) + (c1 * s2_s3);
 m[8] =  (c1 * c2);
}

inline void vector3D_print(const float* v)
{
 std::cout << "<" << v[0] << ", " << v[1] << ", " << v[2] << ">" << std::endl;
}

int main()
{
 AABB_halfdim aabb;
 aabb.center[0] = 0.0f;
 aabb.center[1] = 0.0f;
 aabb.center[2] = 0.0f;
 aabb.widths[0] = 1.0f;
 aabb.widths[1] = 1.0f;
 aabb.widths[2] = 1.0f;

 float v[3] = { 0.3f, 0.1f, 1.1f };
 std::cout << AABB_intersect(aabb, v) << std::endl;

 // define OBB
 OBB obb;
 obb.center[0] = 3.0f; obb.center[1] = 2.0f; obb.center[2] = 1.5f;
 obb.widths[0] = 1.0f; obb.widths[1] = 1.0f; obb.widths[2] = 1.0f;
 float R[9];
 matrix3D_rotate_XYZ(R, 0.0f, 0.0f, radians(46.6f));
 obb.x[0] = R[0]; obb.x[1] = R[1]; obb.x[2] = R[2];
 obb.y[0] = R[3]; obb.y[1] = R[4]; obb.y[2] = R[5];
 obb.z[0] = R[6]; obb.z[1] = R[7]; obb.z[2] = R[8];

 // <2.96051, 3.41366, 2.5> OK
 // <2.96051, 3.41366, 0.5>
 // <4.41366, 2.03949, 2.5> OK
 // <4.41366, 2.03949, 0.5>
 // <1.58634, 1.96051, 2.5> OK
 // <1.58634, 1.96051, 0.5>
 // <3.03949, 0.586338, 2.5> OK
 // <3.03949, 0.586338, 0.5>

 // print vertices to double-check
 float p[8][3];
 OBB_vertices(obb, p);
 for(int i = 0; i < 8; i++)
     vector3D_print(p[i]);

 // define point that doesn't intersect
 v[0] = 4.2f;
 v[1] = 1.4f;
 v[2] = 1.0f;

 // test point
 bool intersect = OBB_intersect(obb, v);
 if(intersect) std::cout << "OBB intersects with point" << std::endl;
 else std::cout << "OBB does not intersect with point" << std::endl;

 // define point that does intersect
 v[0] = 3.8f;
 v[1] = 1.4f;
 v[2] = 1.0f;

 // test point
 intersect = OBB_intersect(obb, v);
 if(intersect) std::cout << "OBB intersects with point" << std::endl;
 else std::cout << "OBB does not intersect with point" << std::endl;

 return 0;
}