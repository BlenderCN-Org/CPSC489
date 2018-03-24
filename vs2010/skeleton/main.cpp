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
 POINT v_abs;
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
MATRIX MatrixTranslation(float x, float y, float z);
void ZeroMatrixTranslation(MATRIX& m);
MATRIX MatrixEulerXYZ(const EULERXYZ& e);
float MatrixInverse(MATRIX& X, const MATRIX& M);
POINT operator +(const POINT& A, const POINT& B);
POINT operator -(const POINT& A, const POINT& B);
MATRIX operator *(const MATRIX& A, const MATRIX& B);
POINT operator *(const MATRIX& A, const POINT& B);
void MatrixPrint(const MATRIX& m);
void Print(const QUATERNION& q);
void VectorPrint(const POINT& v);

int main()
{
 // C++ (requires conjugate to match Blender)
 // q<0.800641, 0.480384, 0.320256, 0.160128>
 cout << "T1" << endl;
 QUATERNION t1;
 t1.q[0] = +5.0f;
 t1.q[1] = -3.0f;
 t1.q[2] = -2.0f;
 t1.q[3] = -1.0f;
 t1 = Normalize(t1);
 Print(t1);
 MatrixPrint(QuaternionToMatrix(t1));

 // q<0.46188, 0.11547, 0.34641, 0.80829>
 cout << "T2" << endl;
 QUATERNION t2;
 t2.q[0] = +4.0f;
 t2.q[1] = -1.0f;
 t2.q[2] = -3.0f;
 t2.q[3] = -7.0f;
 t2 = Normalize(t2);
 Print(t2);
 MatrixPrint(QuaternionToMatrix(t2));

 // q<0.0803219, 0.642575, 0.722897, 0.240966>
 cout << "T3" << endl;
 QUATERNION t3;
 t3.q[0] = +1.0f;
 t3.q[1] = -8.0f;
 t3.q[2] = -9.0f;
 t3.q[3] = -3.0f;
 t3 = Normalize(t3);
 Print(t3);
 MatrixPrint(QuaternionToMatrix(t3));

 //
 // PHASE #1
 // LOAD JOINT SAMPLE DATA FROM BLENDER
 //

 // joint[0]
 // | 1  0 0 0 |
 // | 0  0 1 0 |
 // | 0 -1 0 0 |
 // | 0  0 0 1 |
 jntdata[0].parent = 0xFFFFFFFFul;
 jntdata[0].v_abs.v[0] = 0.0f;
 jntdata[0].v_abs.v[1] = 0.0f;
 jntdata[0].v_abs.v[2] = 0.0f;
 jntdata[0].m_abs.m[0x0] = 1.0f; jntdata[0].m_abs.m[0x1] = 0.0f; jntdata[0].m_abs.m[0x2] = 0.0f; jntdata[0].m_abs.m[0x3] = jntdata[0].v_abs.v[0];
 jntdata[0].m_abs.m[0x4] = 0.0f; jntdata[0].m_abs.m[0x5] = 0.0f; jntdata[0].m_abs.m[0x6] = 1.0f; jntdata[0].m_abs.m[0x7] = jntdata[0].v_abs.v[1];
 jntdata[0].m_abs.m[0x8] = 0.0f; jntdata[0].m_abs.m[0x9] =-1.0f; jntdata[0].m_abs.m[0xA] = 0.0f; jntdata[0].m_abs.m[0xB] = jntdata[0].v_abs.v[2];
 jntdata[0].m_abs.m[0xC] = 0.0f; jntdata[0].m_abs.m[0xD] = 0.0f; jntdata[0].m_abs.m[0xE] = 0.0f; jntdata[0].m_abs.m[0xF] = 1.0f;

 // joint[1]
 jntdata[1].parent = 0;
 jntdata[1].v_abs.v[0] = 0.0f;
 jntdata[1].v_abs.v[1] = 0.0f;
 jntdata[1].v_abs.v[2] = 1.0f;
 jntdata[1].m_abs.m[0x0] = 1.0f; jntdata[1].m_abs.m[0x1] = 0.0f; jntdata[1].m_abs.m[0x2] = 0.0f; jntdata[1].m_abs.m[0x3] = jntdata[1].v_abs.v[0];
 jntdata[1].m_abs.m[0x4] = 0.0f; jntdata[1].m_abs.m[0x5] = 1.0f; jntdata[1].m_abs.m[0x6] = 0.0f; jntdata[1].m_abs.m[0x7] = jntdata[1].v_abs.v[1];
 jntdata[1].m_abs.m[0x8] = 0.0f; jntdata[1].m_abs.m[0x9] = 0.0f; jntdata[1].m_abs.m[0xA] = 1.0f; jntdata[1].m_abs.m[0xB] = jntdata[1].v_abs.v[2];
 jntdata[1].m_abs.m[0xC] = 0.0f; jntdata[1].m_abs.m[0xD] = 0.0f; jntdata[1].m_abs.m[0xE] = 0.0f; jntdata[1].m_abs.m[0xF] = 1.0f;

 // joint[2]
 jntdata[2].parent = 1;
 jntdata[2].v_abs.v[0] = 0.0f;
 jntdata[2].v_abs.v[1] = 1.0f;
 jntdata[2].v_abs.v[2] = 1.0f;
 jntdata[2].m_abs.m[0x0] = 0.0f; jntdata[2].m_abs.m[0x1] =-1.0f; jntdata[2].m_abs.m[0x2] = 0.0f; jntdata[2].m_abs.m[0x3] = jntdata[2].v_abs.v[0];
 jntdata[2].m_abs.m[0x4] = 1.0f; jntdata[2].m_abs.m[0x5] = 0.0f; jntdata[2].m_abs.m[0x6] = 0.0f; jntdata[2].m_abs.m[0x7] = jntdata[2].v_abs.v[1];
 jntdata[2].m_abs.m[0x8] = 0.0f; jntdata[2].m_abs.m[0x9] = 0.0f; jntdata[2].m_abs.m[0xA] = 1.0f; jntdata[2].m_abs.m[0xB] = jntdata[2].v_abs.v[2];
 jntdata[2].m_abs.m[0xC] = 0.0f; jntdata[2].m_abs.m[0xD] = 0.0f; jntdata[2].m_abs.m[0xE] = 0.0f; jntdata[2].m_abs.m[0xF] = 1.0f;

 // set inverse matrices
 for(int i = 0; i < n_jnts; i++)
     MatrixInverse(jntdata[i].m_inv, jntdata[i].m_abs);

 // set relative transformations
 jntdata[0].m_rel = jntdata[0].m_abs;
 jntdata[1].m_rel = jntdata[1].m_abs*jntdata[0].m_inv;
 jntdata[2].m_rel = jntdata[2].m_abs*jntdata[1].m_inv;

 cout << "RELATIVE MATRICES" << endl;
 MatrixPrint(jntdata[0].m_rel);
 MatrixPrint(jntdata[1].m_rel);
 MatrixPrint(jntdata[2].m_rel);

 cout << "RECOMPUTE ABSOLUTE" << endl;
 MatrixPrint(jntdata[0].m_rel);
 MatrixPrint(jntdata[1].m_rel * jntdata[0].m_rel);
 MatrixPrint(jntdata[2].m_rel * jntdata[1].m_rel * jntdata[0].m_rel);

 //
 // PHASE #2
 // LOAD JOINT TRANSFORM DATA FROM BLENDER
 //

 // transform #1
 transforms[0].q[0] = t1.q[0]; // take conjugate
 transforms[0].q[1] = t1.q[1]; // take conjugate
 transforms[0].q[2] = t1.q[2]; // take conjugate
 transforms[0].q[3] = t1.q[3]; // take conjugate
 Normalize(transforms[0]);
 euler[0].x = Radians(-69.0754f);
 euler[0].y = Radians(-21.0372f);
 euler[0].z = Radians(-37.1847f);

 // transform #2
 transforms[1].q[0] = t2.q[0];
 transforms[1].q[1] = t2.q[1];
 transforms[1].q[2] = t2.q[2];
 transforms[1].q[3] = t2.q[3];
 Normalize(transforms[1]);
 euler[1].x = Radians(-42.2737f);
 euler[1].y = Radians(-7.66226f);
 euler[1].z = Radians(-123.476f);

 // transform #3
 transforms[2].q[0] = t3.q[0];
 transforms[2].q[1] = t3.q[1];
 transforms[2].q[2] = t3.q[2];
 transforms[2].q[3] = t3.q[3];
 Normalize(transforms[2]);
 euler[2].x = Radians(-152.592f);
 euler[2].y = Radians(+11.16f);
 euler[2].z = Radians(-99.4623f);

 // point to transform
 point.v[0] = 0.601336f;
 point.v[1] = 0.404549f;
 point.v[2] = 1.112650f;

 // point transformed in Blender
 point_blender.v[0] =-0.612857f;
 point_blender.v[1] =-1.16559f;
 point_blender.v[2] = 0.58993f;

 auto R0 = QuaternionToMatrix(transforms[0]);
 auto R1 = QuaternionToMatrix(transforms[1]);
 auto R2 = QuaternionToMatrix(transforms[2]);
 cout << "ROTATION MATRICES" << endl;
 MatrixPrint(R0); cout << endl;
 MatrixPrint(R1); cout << endl;
 MatrixPrint(R2); cout << endl;

 auto P0 = R0 * jntdata[0].m_rel;
 auto P1 = R1 * jntdata[1].m_rel * P0;
 auto P2 = R2 * jntdata[2].m_rel * P1;
 cout << "POSE MATRICES" << endl;
 MatrixPrint(P0); cout << endl;
 MatrixPrint(P1); cout << endl;
 MatrixPrint(P2); cout << endl;

 cout << "CURRENT POSE MATRICES" << endl;
 MatrixPrint(P0 * jntdata[0].m_inv); cout << endl;
 MatrixPrint(P1 * jntdata[1].m_inv); cout << endl;
 MatrixPrint(P2 * jntdata[2].m_inv); cout << endl;

 //
 // PHASE #6
 // MULTIPLY SAMPLE POINT
 //

 point_transformed = (P1*jntdata[1].m_inv)*point;
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

MATRIX MatrixTranslation(float x, float y, float z)
{
 MATRIX m;
 m.m[0x0] = 1.0f; m.m[0x1] = 0.0f; m.m[0x2] = 0.0f; m.m[0x3] = x;
 m.m[0x4] = 0.0f; m.m[0x5] = 1.0f; m.m[0x6] = 0.0f; m.m[0x7] = y;
 m.m[0x8] = 0.0f; m.m[0x9] = 0.0f; m.m[0xA] = 1.0f; m.m[0xB] = z;
 m.m[0xC] = 0.0f; m.m[0xD] = 0.0f; m.m[0xE] = 0.0f; m.m[0xF] = 1.0f;
 return m;
}

void ZeroMatrixTranslation(MATRIX& m)
{
 m.m[0x3] = 0.0f;
 m.m[0x7] = 0.0f;
 m.m[0xB] = 0.0f;
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
 MATRIX rv;
 rv.m[0x0] = A.m[0x0]*B.m[0x0] + A.m[0x1]*B.m[0x4] + A.m[0x2]*B.m[0x8] + A.m[0x3]*B.m[0xC];
 rv.m[0x1] = A.m[0x0]*B.m[0x1] + A.m[0x1]*B.m[0x5] + A.m[0x2]*B.m[0x9] + A.m[0x3]*B.m[0xD];
 rv.m[0x2] = A.m[0x0]*B.m[0x2] + A.m[0x1]*B.m[0x6] + A.m[0x2]*B.m[0xA] + A.m[0x3]*B.m[0xE];
 rv.m[0x3] = A.m[0x0]*B.m[0x3] + A.m[0x1]*B.m[0x7] + A.m[0x2]*B.m[0xB] + A.m[0x3]*B.m[0xF];
 rv.m[0x4] = A.m[0x4]*B.m[0x0] + A.m[0x5]*B.m[0x4] + A.m[0x6]*B.m[0x8] + A.m[0x7]*B.m[0xC];
 rv.m[0x5] = A.m[0x4]*B.m[0x1] + A.m[0x5]*B.m[0x5] + A.m[0x6]*B.m[0x9] + A.m[0x7]*B.m[0xD];
 rv.m[0x6] = A.m[0x4]*B.m[0x2] + A.m[0x5]*B.m[0x6] + A.m[0x6]*B.m[0xA] + A.m[0x7]*B.m[0xE];
 rv.m[0x7] = A.m[0x4]*B.m[0x3] + A.m[0x5]*B.m[0x7] + A.m[0x6]*B.m[0xB] + A.m[0x7]*B.m[0xF];
 rv.m[0x8] = A.m[0x8]*B.m[0x0] + A.m[0x9]*B.m[0x4] + A.m[0xA]*B.m[0x8] + A.m[0xB]*B.m[0xC];
 rv.m[0x9] = A.m[0x8]*B.m[0x1] + A.m[0x9]*B.m[0x5] + A.m[0xA]*B.m[0x9] + A.m[0xB]*B.m[0xD];
 rv.m[0xA] = A.m[0x8]*B.m[0x2] + A.m[0x9]*B.m[0x6] + A.m[0xA]*B.m[0xA] + A.m[0xB]*B.m[0xE];
 rv.m[0xB] = A.m[0x8]*B.m[0x3] + A.m[0x9]*B.m[0x7] + A.m[0xA]*B.m[0xB] + A.m[0xB]*B.m[0xF];
 rv.m[0xC] = A.m[0xC]*B.m[0x0] + A.m[0xD]*B.m[0x4] + A.m[0xE]*B.m[0x8] + A.m[0xF]*B.m[0xC];
 rv.m[0xD] = A.m[0xC]*B.m[0x1] + A.m[0xD]*B.m[0x5] + A.m[0xE]*B.m[0x9] + A.m[0xF]*B.m[0xD];
 rv.m[0xE] = A.m[0xC]*B.m[0x2] + A.m[0xD]*B.m[0x6] + A.m[0xE]*B.m[0xA] + A.m[0xF]*B.m[0xE];
 rv.m[0xF] = A.m[0xC]*B.m[0x3] + A.m[0xD]*B.m[0x7] + A.m[0xE]*B.m[0xB] + A.m[0xF]*B.m[0xF];
 return rv;
}

POINT operator +(const POINT& A, const POINT& B)
{
 POINT rv;
 rv.v[0] = A.v[0] + B.v[0];
 rv.v[1] = A.v[1] + B.v[1];
 rv.v[2] = A.v[2] + B.v[2];
 return rv;
}

POINT operator -(const POINT& A, const POINT& B)
{
 POINT rv;
 rv.v[0] = A.v[0] - B.v[0];
 rv.v[1] = A.v[1] - B.v[1];
 rv.v[2] = A.v[2] - B.v[2];
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

/*
m1_abs = [1  0 0 0; 0 0 1 0; 0 -1 0 0; 0 0 0 1]
m2_abs = [1  0 0 0; 0 1 0 0; 0  0 1 1; 0 0 0 1]
m3_abs = [0 -1 0 0; 1 0 0 1; 0  0 1 1; 0 0 0 1]

m1_inv = inv(m1_abs)
m2_inv = inv(m2_abs)
m3_inv = inv(m3_abs)

m1_rel = m1_abs
m2_rel = m2_abs*m1_inv
m3_rel = m3_abs*m2_inv

t1 = [5 -3 -2 -1]
t2 = [4 -1 -3 -7]
t3 = [1 -8 -9 -3]
t1 = quatnormalize(t1)
t2 = quatnormalize(t2)
t3 = quatnormalize(t3)

R0 = quat2dcm(t1)
R0 = transpose(R0)
R0(1, 4) = 0
R0(2, 4) = 0
R0(3, 4) = 0
R0(4, 1) = 0
R0(4, 2) = 0
R0(4, 3) = 0
R0(4, 4) = 1

R1 = quat2dcm(t2)
R1 = transpose(R1)
R1(1, 4) = 0
R1(2, 4) = 0
R1(3, 4) = 0
R1(4, 1) = 0
R1(4, 2) = 0
R1(4, 3) = 0
R1(4, 4) = 1

R2 = quat2dcm(t3)
R2 = transpose(R2)
R2(1, 4) = 0
R2(2, 4) = 0
R2(3, 4) = 0
R2(4, 1) = 0
R2(4, 2) = 0
R2(4, 3) = 0
R2(4, 4) = 1

P0 = R0 * m1_rel
P1 = R1 * m2_rel * P0
P2 = R2 * m3_rel * P1

S0 = P0 * m1_inv
S1 = P1 * m2_inv
S2 = P2 * m2_inv

point = S1 * [0.601336; 0.404549; 1.112650; 1]
*/