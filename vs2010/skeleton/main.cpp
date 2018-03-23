#include<iostream>
#include<cmath>
using namespace std;

struct EULERXYZ {
 float x;
 float y;
 float z;
};
struct QUATERNION {
 float q[4];
};
struct POINT {
 float v[3];
};
struct MATRIX {
 float m[16];
 MATRIX() {}
 MATRIX(const MATRIX& other) {
  m[0x0] = other.m[0x0]; m[0x1] = other.m[0x1]; m[0x2] = other.m[0x2]; m[0x3] = other.m[0x3];
  m[0x4] = other.m[0x4]; m[0x5] = other.m[0x5]; m[0x6] = other.m[0x6]; m[0x7] = other.m[0x7];
  m[0x8] = other.m[0x8]; m[0x9] = other.m[0x9]; m[0xA] = other.m[0xA]; m[0xB] = other.m[0xB];
  m[0xC] = other.m[0xC]; m[0xD] = other.m[0xD]; m[0xE] = other.m[0xE]; m[0xF] = other.m[0xF];
 }
 MATRIX& operator =(const MATRIX& other) {
  if(this == &other) return *this;
  m[0x0] = other.m[0x0]; m[0x1] = other.m[0x1]; m[0x2] = other.m[0x2]; m[0x3] = other.m[0x3];
  m[0x4] = other.m[0x4]; m[0x5] = other.m[0x5]; m[0x6] = other.m[0x6]; m[0x7] = other.m[0x7];
  m[0x8] = other.m[0x8]; m[0x9] = other.m[0x9]; m[0xA] = other.m[0xA]; m[0xB] = other.m[0xB];
  m[0xC] = other.m[0xC]; m[0xD] = other.m[0xD]; m[0xE] = other.m[0xE]; m[0xF] = other.m[0xF];
  return *this;
 }
};
struct JOINT {
 unsigned int parent;
 MATRIX m_abs;
 MATRIX m_inv;
 MATRIX m_rel;
 MATRIX m_pos;
};
static const int n_jnts = 3;
static JOINT jntdata[n_jnts];
static QUATERNION transforms[3];
static EULERXYZ euler[3];
static POINT point;
static POINT point_transformed;
static POINT point_blender;
static POINT joint_blender[3];


//
// REQUIREMENTS
//
inline float Radians(float d) { return d*0.01745329252f; }
inline float Degrees(float r) { return r*57.2957795131f; }
QUATERNION Normalize(const QUATERNION& q);
MATRIX QuaternionToMatrix(const QUATERNION& q);
MATRIX MatrixEulerXYZ(const EULERXYZ& e);
float MatrixInverse(MATRIX& X, const MATRIX& M);
MATRIX operator *(const MATRIX& A, const MATRIX& B);
POINT operator *(const MATRIX& A, const POINT& B);
void MatrixPrint(const MATRIX& m);
void Print(const QUATERNION& q);
void VectorPrint(const POINT& v);

int main()
{
 QUATERNION t1;
 t1.q[0] = 5.0f;
 t1.q[1] = 3.0f;
 t1.q[2] = 2.0f;
 t1.q[3] = 1.0f;
 t1 = Normalize(t1);
 Print(t1);
 MatrixPrint(QuaternionToMatrix(t1));
 
 // C++ (requires conjugate to match Blender)
 // q<0.800641, 0.480384, 0.320256, 0.160128>
 // | 0.743590 0.051282  0.666667 | x
 // | 0.564103 0.487179 -0.666667 | y
 // |-0.358974 0.871795  0.333333 | z
 //
 // Blender
 // This is verified to be the pose bone matrix.
 // It is column major order.
 // w=0.8006, x=0.4804, y=0.3203, z=0.1601
 //      x       y        z
 // | 0.7436,  0.0513,  0.6667 |
 // | 0.3590, -0.8718, -0.3333 |
 // | 0.5641,  0.4872, -0.6667 |


 //
 // PHASE #1
 // LOAD JOINT SAMPLE DATA FROM BLENDER
 //

 // joint[0]
 jntdata[0].parent = 0xFFFFFFFFul;
 jntdata[0].m_abs.m[0x0] = 1.0f; // x is +x
 jntdata[0].m_abs.m[0x1] = 0.0f;
 jntdata[0].m_abs.m[0x2] = 0.0f;
 jntdata[0].m_abs.m[0x3] = 0.0f;
 jntdata[0].m_abs.m[0x4] = 0.0f; // y is +z
 jntdata[0].m_abs.m[0x5] = 0.0f;
 jntdata[0].m_abs.m[0x6] = 1.0f;
 jntdata[0].m_abs.m[0x7] = 0.0f;
 jntdata[0].m_abs.m[0x8] = 0.0f; // z is -y
 jntdata[0].m_abs.m[0x9] =-1.0f;
 jntdata[0].m_abs.m[0xA] = 0.0f;
 jntdata[0].m_abs.m[0xB] = 0.0f;
 jntdata[0].m_abs.m[0xC] = 0.0f;
 jntdata[0].m_abs.m[0xD] = 0.0f;
 jntdata[0].m_abs.m[0xE] = 0.0f;
 jntdata[0].m_abs.m[0xF] = 1.0f;

 // joint[1]
 jntdata[1].parent = 0;
 jntdata[1].m_abs.m[0x0] = 1.0f; // x is +x
 jntdata[1].m_abs.m[0x1] = 0.0f;
 jntdata[1].m_abs.m[0x2] = 0.0f;
 jntdata[1].m_abs.m[0x3] = 0.0f;
 jntdata[1].m_abs.m[0x4] = 0.0f; // y is +y
 jntdata[1].m_abs.m[0x5] = 1.0f;
 jntdata[1].m_abs.m[0x6] = 0.0f;
 jntdata[1].m_abs.m[0x7] = 0.0f;
 jntdata[1].m_abs.m[0x8] = 0.0f; // z is +z
 jntdata[1].m_abs.m[0x9] = 0.0f;
 jntdata[1].m_abs.m[0xA] = 1.0f;
 jntdata[1].m_abs.m[0xB] = 1.0f;
 jntdata[1].m_abs.m[0xC] = 0.0f;
 jntdata[1].m_abs.m[0xD] = 0.0f;
 jntdata[1].m_abs.m[0xE] = 0.0f;
 jntdata[1].m_abs.m[0xF] = 1.0f;

 // joint[2]
 jntdata[2].parent = 1;
 jntdata[2].m_abs.m[0x0] = 0.0f; // x is -y
 jntdata[2].m_abs.m[0x1] =-1.0f;
 jntdata[2].m_abs.m[0x2] = 0.0f;
 jntdata[2].m_abs.m[0x3] = 0.0f;
 jntdata[2].m_abs.m[0x4] = 1.0f; // y is +x
 jntdata[2].m_abs.m[0x5] = 0.0f;
 jntdata[2].m_abs.m[0x6] = 0.0f;
 jntdata[2].m_abs.m[0x7] = 1.0f;
 jntdata[2].m_abs.m[0x8] = 0.0f; // z is +z
 jntdata[2].m_abs.m[0x9] = 0.0f;
 jntdata[2].m_abs.m[0xA] = 1.0f;
 jntdata[2].m_abs.m[0xB] = 1.0f;
 jntdata[2].m_abs.m[0xC] = 0.0f;
 jntdata[2].m_abs.m[0xD] = 0.0f;
 jntdata[2].m_abs.m[0xE] = 0.0f;
 jntdata[2].m_abs.m[0xF] = 1.0f;

 // set inverse matrices
 MatrixInverse(jntdata[0].m_inv, jntdata[0].m_abs);
 MatrixInverse(jntdata[1].m_inv, jntdata[1].m_abs);
 MatrixInverse(jntdata[2].m_inv, jntdata[2].m_abs);

 // set relative matrices
 jntdata[0].m_rel = jntdata[0].m_abs;
 jntdata[1].m_rel = jntdata[1].m_abs*jntdata[0].m_inv;
 jntdata[2].m_rel = jntdata[2].m_abs*jntdata[1].m_inv;

 //
 // PHASE #2
 // LOAD JOINT TRANSFORM DATA FROM BLENDER
 //

 // transform #1
 transforms[0].q[0] = 0.707107f;
 transforms[0].q[1] = 0.0f;
 transforms[0].q[2] =-0.707107f;
 transforms[0].q[3] = 0.0f;
 Normalize(transforms[0]);
 euler[0].x = Radians( 0.0f);
 euler[0].y = Radians(-90.0f);
 euler[0].z = Radians( 0.0f);

 // transform #2
 transforms[1].q[0] = 1.0f;
 transforms[1].q[1] = 0.0f;
 transforms[1].q[2] = 0.0f;
 transforms[1].q[3] = 0.0f;
 Normalize(transforms[1]);
 euler[1].x = Radians(0.0f);
 euler[1].y = Radians(0.0f);
 euler[1].z = Radians(0.0f);

 // transform #3
 transforms[2].q[0] = 1.0f;
 transforms[2].q[1] = 0.0f;
 transforms[2].q[2] = 0.0f;
 transforms[2].q[3] = 0.0f;
 Normalize(transforms[2]);
 euler[2].x = Radians(0.0f);
 euler[2].y = Radians(0.0f);
 euler[2].z = Radians(0.0f);

 // point to transform
 point.v[0] = 0.601336f;
 point.v[1] = 0.404549f;
 point.v[2] = 1.112650f;

 // point transformed in Blender
 point_blender.v[0] = 0.40455f;
 point_blender.v[1] =-0.601336f;
 point_blender.v[2] = 1.11265f;

 // joint[0] transformed in Blender
 joint_blender[0].v[0] = 0.0f;
 joint_blender[0].v[1] = 0.0f;
 joint_blender[0].v[2] = 0.0f;

 // joint[1] transformed in Blender
 joint_blender[1].v[0] = 0.0f;
 joint_blender[1].v[1] = 0.0f;
 joint_blender[1].v[2] = 1.0f;

 // joint[2] transformed in Blender
 joint_blender[2].v[0] = 1.0f;
 joint_blender[2].v[1] = 0.0f;
 joint_blender[2].v[2] = 1.0f;

 //
 // PHASE #3
 // COMPUTE INVERSE BIND POSE MATRICES
 //

 for(int i = 0; i < n_jnts; i++)
     MatrixInverse(jntdata[i].m_inv, jntdata[i].m_abs);

//  //
//  // PHASE #4
//  // COMPUTE CURRENT POSE MATRICES
//  //
// 
//  // propagate transformation matrices through skeleton tree
//  jntdata[0].m_rel = QuaternionToMatrix(transforms[0]);
//  for(int i = 1; i < n_jnts; i++) {
//      MATRIX R = QuaternionToMatrix(transforms[i]);
//      jntdata[i].m_rel = R * jntdata[jntdata[i].parent].m_rel;
//     }
// 
//  //
//  // PHASE #5
//  // MULTIPLY CURRENT POSE AND INVERSE BIND POSE MATRICES
//  //
// 
//  for(int i = 0; i < n_jnts; i++)
//      jntdata[i].m_rel = jntdata[i].m_abs * jntdata[i].m_rel * jntdata[i].m_inv;

    // propagate transformation matrices through skeleton tree
    jntdata[0].m_pos = QuaternionToMatrix(transforms[0]);
    cout << "Concatenated transform[0]" << endl;
    MatrixPrint(jntdata[0].m_pos);
    for(int i = 1; i < n_jnts; i++) {
        MATRIX R = QuaternionToMatrix(transforms[i]);
        jntdata[i].m_pos = jntdata[jntdata[i].parent].m_pos * R;
        cout << "Concatenated transform[" << i << "]" << endl;
        MatrixPrint(jntdata[i].m_pos);
       }

    // if all transformations are identity matrices, everything returns back to m_abs
    for(int i = 0; i < n_jnts; i++) {
        jntdata[i].m_pos = jntdata[i].m_abs * jntdata[i].m_pos * jntdata[i].m_inv;
        cout << "Pose transform[" << i << "]" << endl;
        MatrixPrint(jntdata[i].m_pos);
       }

 //
 // PHASE #6
 // MULTIPLY SAMPLE POINT
 //

 POINT p = point;
 point_transformed = jntdata[0].m_pos * p;
 cout << "Calculated: " << endl;
 VectorPrint(point_transformed);
 cout << "Expected: " << endl;
 VectorPrint(point_blender);

 return -1;
}

QUATERNION Normalize(const QUATERNION& q)
{
 float scalar = 1.0f/sqrt(q.q[0]*q.q[0] + q.q[1]*q.q[1] + q.q[2]*q.q[2] + q.q[3]*q.q[3]);
 QUATERNION rv;
 rv.q[0] = q.q[0]*scalar;
 rv.q[1] = q.q[1]*scalar;
 rv.q[2] = q.q[2]*scalar;
 rv.q[3] = q.q[3]*scalar;
 return rv;
}

MATRIX QuaternionToMatrix(const QUATERNION& q)
{
 // constants
 float zer = 0.0f;
 float one = 1.0f;

 // factors
 float a2 = q.q[0] + q.q[0];
 float b2 = q.q[1] + q.q[1];
 float c2 = q.q[2] + q.q[2];
 float d2 = q.q[3] + q.q[3];

 // factors
 float ab2 = q.q[0] * b2;
 float ac2 = q.q[0] * c2;
 float ad2 = q.q[0] * d2;
 float bb2 = q.q[1] * b2;
 float bc2 = q.q[1] * c2;
 float bd2 = q.q[1] * d2;
 float cc2 = q.q[2] * c2;
 float cd2 = q.q[2] * d2;
 float dd2 = q.q[3] * d2;

 // reinterpret matrix
 MATRIX rv;
 rv.m[0x0] = one - cc2 - dd2;  // correct
 rv.m[0x1] = bc2 - ad2;        // correct
 rv.m[0x2] = bd2 + ac2;        // correct
 rv.m[0x3] = 0.0f;
 rv.m[0x4] = bc2 + ad2;        // correct
 rv.m[0x5] = 1.0f - bb2 - dd2; // correct
 rv.m[0x6] = cd2 - ab2;        // correct
 rv.m[0x7] = 0.0f;
 rv.m[0x8] = bd2 - ac2;        // correct
 rv.m[0x9] = cd2 + ab2;        // correct
 rv.m[0xA] = 1.0f - bb2 - cc2; // correct
 rv.m[0xB] = 0.0f;
 rv.m[0xC] = 0.0f;
 rv.m[0xD] = 0.0f;
 rv.m[0xE] = 0.0f;
 rv.m[0xF] = 1.0f;
 return rv;
}

MATRIX MatrixEulerXYZ(const EULERXYZ& e)
{
 // cached cosines
 float c1 = cos(e.x);
 float c2 = cos(e.y);
 float c3 = cos(e.z);

 // cached sines
 float s1 = sin(e.x);
 float s2 = sin(e.y);
 float s3 = sin(e.z);

 // composite values
 float c3_s2 = c3 * s2;
 float s2_s3 = s2 * s3;

 // set matrix
 MATRIX m;
 m.m[0x0] =  (c2 * c3);
 m.m[0x1] = -(c2 * s3);
 m.m[0x2] =  s2;
 m.m[0x3] =  0.0f;
 m.m[0x4] =  (c1 * s3) + (s1 * c3_s2);
 m.m[0x5] =  (c1 * c3) - (s1 * s2_s3);
 m.m[0x6] = -(c2 * s1);
 m.m[0x7] =  0.0f;
 m.m[0x8] =  (s1 * s3) - (c1 * c3_s2);
 m.m[0x9] =  (c3 * s1) + (c1 * s2_s3);
 m.m[0xA] =  (c1 * c2);
 m.m[0xB] =  0.0f;
 m.m[0xC] =  0.0f;
 m.m[0xD] =  0.0f;
 m.m[0xE] =  0.0f;
 m.m[0xF] =  1.0f;
 return m;
}

float MatrixInverse(MATRIX& X, const MATRIX& M)
{
 // cache 2x2 determinants
 float D[12] = {
  M.m[0xA]*M.m[0xF] - M.m[0xB]*M.m[0xE],
  M.m[0x9]*M.m[0xF] - M.m[0xB]*M.m[0xD],
  M.m[0x9]*M.m[0xE] - M.m[0xA]*M.m[0xD],
  M.m[0x8]*M.m[0xF] - M.m[0xB]*M.m[0xC],
  M.m[0x8]*M.m[0xE] - M.m[0xA]*M.m[0xC],
  M.m[0x8]*M.m[0xD] - M.m[0x9]*M.m[0xC],
  M.m[0x2]*M.m[0x7] - M.m[0x3]*M.m[0x6],
  M.m[0x1]*M.m[0x7] - M.m[0x3]*M.m[0x5],
  M.m[0x1]*M.m[0x6] - M.m[0x2]*M.m[0x5],
  M.m[0x0]*M.m[0x7] - M.m[0x3]*M.m[0x4],
  M.m[0x0]*M.m[0x6] - M.m[0x2]*M.m[0x4],
  M.m[0x0]*M.m[0x5] - M.m[0x1]*M.m[0x4]
 };

 // compute determinant
 float det = D[0x5]*D[0x6] - D[0x4]*D[0x7] + D[0x3]*D[0x8] + D[0x2]*D[0x9] - D[0x1]*D[0xA] + D[0x0]*D[0xB];
 if(std::fabs(det) < 1.0e-7f) return det;

 // inv(M) = inv(det(M)) * transpose(C)
 float invdet = 1.0f/det;
 X.m[0x0] = +(M.m[0x5]*D[0x0] - M.m[0x6]*D[0x1] + M.m[0x7]*D[0x2])*invdet;
 X.m[0x4] = -(M.m[0x4]*D[0x0] - M.m[0x6]*D[0x3] + M.m[0x7]*D[0x4])*invdet;
 X.m[0x8] = +(M.m[0x4]*D[0x1] - M.m[0x5]*D[0x3] + M.m[0x7]*D[0x5])*invdet;
 X.m[0xC] = -(M.m[0x4]*D[0x2] - M.m[0x5]*D[0x4] + M.m[0x6]*D[0x5])*invdet;
 X.m[0x1] = -(M.m[0x1]*D[0x0] - M.m[0x2]*D[0x1] + M.m[0x3]*D[0x2])*invdet;
 X.m[0x5] = +(M.m[0x0]*D[0x0] - M.m[0x2]*D[0x3] + M.m[0x3]*D[0x4])*invdet;
 X.m[0x9] = -(M.m[0x0]*D[0x1] - M.m[0x1]*D[0x3] + M.m[0x3]*D[0x5])*invdet;
 X.m[0xD] = +(M.m[0x0]*D[0x2] - M.m[0x1]*D[0x4] + M.m[0x2]*D[0x5])*invdet;
 X.m[0x2] = +(M.m[0xD]*D[0x6] - M.m[0xE]*D[0x7] + M.m[0xF]*D[0x8])*invdet;
 X.m[0x6] = -(M.m[0xC]*D[0x6] - M.m[0xE]*D[0x9] + M.m[0xF]*D[0xA])*invdet;
 X.m[0xA] = +(M.m[0xC]*D[0x7] - M.m[0xD]*D[0x9] + M.m[0xF]*D[0xB])*invdet;
 X.m[0xE] = -(M.m[0xC]*D[0x8] - M.m[0xD]*D[0xA] + M.m[0xE]*D[0xB])*invdet;
 X.m[0x3] = -(M.m[0x9]*D[0x6] - M.m[0xA]*D[0x7] + M.m[0xB]*D[0x8])*invdet;
 X.m[0x7] = +(M.m[0x8]*D[0x6] - M.m[0xA]*D[0x9] + M.m[0xB]*D[0xA])*invdet;
 X.m[0xB] = -(M.m[0x8]*D[0x7] - M.m[0x9]*D[0x9] + M.m[0xB]*D[0xB])*invdet;
 X.m[0xF] = +(M.m[0x8]*D[0x8] - M.m[0x9]*D[0xA] + M.m[0xA]*D[0xB])*invdet;

 // return determinant
 return det;
}

MATRIX operator *(const MATRIX& A, const MATRIX& B)
{
 float T[16] = {
  A.m[0x0]*B.m[0x0] + A.m[0x1]*B.m[0x4] + A.m[0x2]*B.m[0x8] + A.m[0x3]*B.m[0xC],
  A.m[0x0]*B.m[0x1] + A.m[0x1]*B.m[0x5] + A.m[0x2]*B.m[0x9] + A.m[0x3]*B.m[0xD],
  A.m[0x0]*B.m[0x2] + A.m[0x1]*B.m[0x6] + A.m[0x2]*B.m[0xA] + A.m[0x3]*B.m[0xE],
  A.m[0x0]*B.m[0x3] + A.m[0x1]*B.m[0x7] + A.m[0x2]*B.m[0xB] + A.m[0x3]*B.m[0xF],
  A.m[0x4]*B.m[0x0] + A.m[0x5]*B.m[0x4] + A.m[0x6]*B.m[0x8] + A.m[0x7]*B.m[0xC],
  A.m[0x4]*B.m[0x1] + A.m[0x5]*B.m[0x5] + A.m[0x6]*B.m[0x9] + A.m[0x7]*B.m[0xD],
  A.m[0x4]*B.m[0x2] + A.m[0x5]*B.m[0x6] + A.m[0x6]*B.m[0xA] + A.m[0x7]*B.m[0xE],
  A.m[0x4]*B.m[0x3] + A.m[0x5]*B.m[0x7] + A.m[0x6]*B.m[0xB] + A.m[0x7]*B.m[0xF],
  A.m[0x8]*B.m[0x0] + A.m[0x9]*B.m[0x4] + A.m[0xA]*B.m[0x8] + A.m[0xB]*B.m[0xC],
  A.m[0x8]*B.m[0x1] + A.m[0x9]*B.m[0x5] + A.m[0xA]*B.m[0x9] + A.m[0xB]*B.m[0xD],
  A.m[0x8]*B.m[0x2] + A.m[0x9]*B.m[0x6] + A.m[0xA]*B.m[0xA] + A.m[0xB]*B.m[0xE],
  A.m[0x8]*B.m[0x3] + A.m[0x9]*B.m[0x7] + A.m[0xA]*B.m[0xB] + A.m[0xB]*B.m[0xF],
  A.m[0xC]*B.m[0x0] + A.m[0xD]*B.m[0x4] + A.m[0xE]*B.m[0x8] + A.m[0xF]*B.m[0xC],
  A.m[0xC]*B.m[0x1] + A.m[0xD]*B.m[0x5] + A.m[0xE]*B.m[0x9] + A.m[0xF]*B.m[0xD],
  A.m[0xC]*B.m[0x2] + A.m[0xD]*B.m[0x6] + A.m[0xE]*B.m[0xA] + A.m[0xF]*B.m[0xE],
  A.m[0xC]*B.m[0x3] + A.m[0xD]*B.m[0x7] + A.m[0xE]*B.m[0xB] + A.m[0xF]*B.m[0xF]
 };
 MATRIX rv;
 rv.m[0x0] = T[0x0];
 rv.m[0x1] = T[0x1];
 rv.m[0x2] = T[0x2];
 rv.m[0x3] = T[0x3];
 rv.m[0x4] = T[0x4];
 rv.m[0x5] = T[0x5];
 rv.m[0x6] = T[0x6];
 rv.m[0x7] = T[0x7];
 rv.m[0x8] = T[0x8];
 rv.m[0x9] = T[0x9];
 rv.m[0xA] = T[0xA];
 rv.m[0xB] = T[0xB];
 rv.m[0xC] = T[0xC];
 rv.m[0xD] = T[0xD];
 rv.m[0xE] = T[0xE];
 rv.m[0xF] = T[0xF];
 return rv;
}

POINT operator *(const MATRIX& A, const POINT& B)
{
 float T[3] = {
  A.m[0x0]*B.v[0x0] + A.m[0x1]*B.v[0x1] + A.m[0x2]*B.v[0x2] + A.m[0x3],
  A.m[0x4]*B.v[0x0] + A.m[0x5]*B.v[0x1] + A.m[0x6]*B.v[0x2] + A.m[0x7],
  A.m[0x8]*B.v[0x0] + A.m[0x9]*B.v[0x1] + A.m[0xA]*B.v[0x2] + A.m[0xB]
 };
 POINT rv;
 rv.v[0x0] = T[0x0];
 rv.v[0x1] = T[0x1];
 rv.v[0x2] = T[0x2];
 return rv;
}

void MatrixPrint(const MATRIX& m)
{
 MATRIX c = m;
 for(int i = 0; i < 16; i++) c.m[i] = (fabs(m.m[i]) < 1.0e-6f ? 0.0f : m.m[i]);
 cout << "|" << c.m[0x0] << " " << c.m[0x1] << " " << c.m[0x2] << " " << c.m[0x3] << "|" << endl;
 cout << "|" << c.m[0x4] << " " << c.m[0x5] << " " << c.m[0x6] << " " << c.m[0x7] << "|" << endl;
 cout << "|" << c.m[0x8] << " " << c.m[0x9] << " " << c.m[0xA] << " " << c.m[0xB] << "|" << endl;
 cout << "|" << c.m[0xC] << " " << c.m[0xD] << " " << c.m[0xE] << " " << c.m[0xF] << "|" << endl;
}

void Print(const QUATERNION& q)
{
 cout << "<" << q.q[0] << ", " << q.q[1] << ", " << q.q[2] << ", " << q.q[3] << ">" << endl;
}

void VectorPrint(const POINT& v)
{
 cout << "<" << v.v[0] << ", " << v.v[1] << ", " << v.v[2] << ">" << endl;
}