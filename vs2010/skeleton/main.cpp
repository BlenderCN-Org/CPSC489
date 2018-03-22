#include<iostream>
#include<cmath>
using namespace std;

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
};
static const int n_jnts = 3;
static JOINT jntdata[n_jnts];
static QUATERNION transforms[3];
static POINT point;
static POINT point_transformed;
static POINT point_blender;
static POINT joint_blender[3];

//
// REQUIREMENTS
//
QUATERNION Normalize(const QUATERNION& q);
MATRIX QuaternionToMatrix(const QUATERNION& q);
float MatrixInverse(MATRIX& X, const MATRIX& M);
MATRIX operator *(const MATRIX& A, const MATRIX& B);
POINT operator *(const MATRIX& A, const POINT& B);
void MatrixPrint(const MATRIX& m);
void VectorPrint(const POINT& v);

int main()
{
 //
 // PHASE #1
 // LOAD JOINT SAMPLE DATA FROM BLENDER
 //

 // joint[0]
 jntdata[0].parent = 0xFFFFFFFFul;
 jntdata[0].m_abs.m[0x0] = 1.0f;
 jntdata[0].m_abs.m[0x1] = 0.0f;
 jntdata[0].m_abs.m[0x2] = 0.0f;
 jntdata[0].m_abs.m[0x3] = 1.0f;
 jntdata[0].m_abs.m[0x4] = 0.0f;
 jntdata[0].m_abs.m[0x5] = 0.4999999701976776f;
 jntdata[0].m_abs.m[0x6] = -0.866025447845459f;
 jntdata[0].m_abs.m[0x7] = 2.0f;
 jntdata[0].m_abs.m[0x8] = 0.0f;
 jntdata[0].m_abs.m[0x9] = 0.866025447845459f;
 jntdata[0].m_abs.m[0xA] = 0.4999999701976776f;
 jntdata[0].m_abs.m[0xB] = 3.0f;
 jntdata[0].m_abs.m[0xC] = 0.0f;
 jntdata[0].m_abs.m[0xD] = 0.0f;
 jntdata[0].m_abs.m[0xE] = 0.0f;
 jntdata[0].m_abs.m[0xF] = 1.0f;

 // joint[1]
 jntdata[1].parent = 0;
 jntdata[1].m_abs.m[0x0] = 1.0f;
 jntdata[1].m_abs.m[0x1] = 0.0f;
 jntdata[1].m_abs.m[0x2] = 0.0f;
 jntdata[1].m_abs.m[0x3] = 1.0f;
 jntdata[1].m_abs.m[0x4] = 0.0f;
 jntdata[1].m_abs.m[0x5] = 0.9551945924758911f;
 jntdata[1].m_abs.m[0x6] = -0.29597848653793335f;
 jntdata[1].m_abs.m[0x7] = 2.5f;
 jntdata[1].m_abs.m[0x8] = 0.0f;
 jntdata[1].m_abs.m[0x9] = 0.29597848653793335f;
 jntdata[1].m_abs.m[0xA] = 0.9551945924758911f;
 jntdata[1].m_abs.m[0xB] = 3.866025447845459f;
 jntdata[1].m_abs.m[0xC] = 0.0f;
 jntdata[1].m_abs.m[0xD] = 0.0f;
 jntdata[1].m_abs.m[0xE] = 0.0f;
 jntdata[1].m_abs.m[0xF] = 1.0f;

 // joint[2]
 jntdata[2].parent = 1;
 jntdata[2].m_abs.m[0x0] = 0.9999998211860657f;
 jntdata[2].m_abs.m[0x1] = 0.0f;
 jntdata[2].m_abs.m[0x2] = 0.000640869140625f;
 jntdata[2].m_abs.m[0x3] = 1.0f;
 jntdata[2].m_abs.m[0x4] = -0.00032257664133794606f;
 jntdata[2].m_abs.m[0x5] = 0.8640871047973633f;
 jntdata[2].m_abs.m[0x6] = 0.5033423900604248f;
 jntdata[2].m_abs.m[0x7] = 3.181468963623047f;
 jntdata[2].m_abs.m[0x8] = -0.0005537667311728001f;
 jntdata[2].m_abs.m[0x9] = -0.5033424496650696f;
 jntdata[2].m_abs.m[0xA] = 0.8640868663787842f;
 jntdata[2].m_abs.m[0xB] = 4.077186584472656f;
 jntdata[2].m_abs.m[0xC] = 0.0f;
 jntdata[2].m_abs.m[0xD] = 0.0f;
 jntdata[2].m_abs.m[0xE] = 0.0f;
 jntdata[2].m_abs.m[0xF] = 1.0f;

 //
 // PHASE #2
 // LOAD JOINT TRANSFORM DATA FROM BLENDER
 //

 // transform #1
 transforms[0].q[0] = 0.845863f;
 transforms[0].q[1] = 0.007272f;
 transforms[0].q[2] = -0.491491f;
 transforms[0].q[3] = -0.207121f;
 Normalize(transforms[0]);

 // transform #2
 transforms[1].q[0] = 0.490048f;
 transforms[1].q[1] = 0.256793f;
 transforms[1].q[2] = 0.686258f;
 transforms[1].q[3] = 0.472187f;
 Normalize(transforms[1]);

 // transform #3
 transforms[2].q[0] = 0.443239f;
 transforms[2].q[1] = 0.819386f;
 transforms[2].q[2] = -0.271397f;
 transforms[2].q[3] = 0.241845f;
 Normalize(transforms[2]);

 // point to transform
 point.v[0] = 1.25f;
 point.v[1] = 3.50f;
 point.v[2] = 4.00f;

 // point transformed in Blender
 point_blender.v[0] = 1.61264f;
 point_blender.v[1] = 2.16104f;
 point_blender.v[2] = 4.69789f;

 // joint[0] transformed in Blender
 joint_blender[0].v[0] = 1.0f;
 joint_blender[0].v[1] = 2.0f;
 joint_blender[0].v[2] = 3.0f;

 // joint[1] transformed in Blender
 joint_blender[1].v[0] = 1.34324f;
 joint_blender[1].v[1] = 2.27007f;
 joint_blender[1].v[2] = 3.89958f;

 // joint[2] transformed in Blender
 joint_blender[2].v[0] = 1.31243f;
 joint_blender[2].v[1] = 2.42429f;
 joint_blender[2].v[2] = 4.59546f;

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
    jntdata[0].m_rel = QuaternionToMatrix(transforms[0]);
    cout << "Concatenated transform[0]" << endl;
    MatrixPrint(jntdata[0].m_rel);
    for(int i = 1; i < n_jnts; i++) {
        MATRIX R = QuaternionToMatrix(transforms[i]);
        jntdata[i].m_rel = R * jntdata[jntdata[i].parent].m_rel;
        cout << "Concatenated transform[" << i << "]" << endl;
        MatrixPrint(jntdata[i].m_rel);
       }

    // if all transformations are identity matrices, everything returns back to m_abs
    for(int i = 0; i < n_jnts; i++) {
        jntdata[i].m_rel = jntdata[i].m_abs * jntdata[i].m_rel;
        cout << "Pose transform[" << i << "]" << endl;
        MatrixPrint(jntdata[i].m_rel);
       }

 //
 // PHASE #6
 // MULTIPLY SAMPLE POINT
 //

 point_transformed = jntdata[2].m_rel * (jntdata[2].m_inv * point);

 auto R1 = QuaternionToMatrix(transforms[0]);
 auto R2 = QuaternionToMatrix(transforms[1]);
 auto R3 = QuaternionToMatrix(transforms[2]);
 point_transformed = (jntdata[2].m_inv*R1*jntdata[2].m_abs)*(jntdata[1].m_inv*R2*jntdata[1].m_abs)*(jntdata[0].m_inv*R3*jntdata[0].m_abs)*point;
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

void VectorPrint(const POINT& v)
{
 cout << "<" << v.v[0] << ", " << v.v[1] << ", " << v.v[2] << ">" << endl;
}