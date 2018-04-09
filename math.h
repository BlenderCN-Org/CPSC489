#ifndef __CS489_MATH_H
#define __CS489_MATH_H

// GENERAL FUNCTIONS
#pragma region GENERAL_FUNCTIONS

inline real32 inv(real32 x)
{
 return 1.0f/x;
}

inline real32 sarrus_2x2(real32 m11, real32 m12, real32 m21, real32 m22)
{
 return m11*m22 - m12*m21;
}

inline real32 sarrus_2x2(const real32* m)
{
 return m[0]*m[3] - m[1]*m[2];
}

#pragma endregion GENERAL_FUNCTIONS

// SPECIAL NUMBERS
#pragma region SPECIAL_NUMBERS

inline real32 epsilon(void)
{
 return 1.0e-7f;
}

inline bool is_zero(real32 value)
{
 return (std::fabs(value) < epsilon());
}

inline real32 positive_infinity(void)
{
 // 0-11111111-00000000000000000000000
 uint32 x = 0x7F800000u;
 return *reinterpret_cast<real32*>(&x);
}

inline real32 negative_infinity(void)
{
 // 1-11111111-00000000000000000000000
 uint32 x = 0xFF800000u;
 return *reinterpret_cast<real32*>(&x);
}

#pragma endregion SPECIAL_NUMBERS

// TRIGNOMETRY
#pragma region TRIGNOMETRY

// Trignometric Constants
inline real32 pi(void)           { uint32 x = 0x40490FDBul; return *reinterpret_cast<real32*>(&x); } // oeis.org code:A000796
inline real32 pi_times_2(void)   { return 6.28318530718f; } // oeis.org code:A019692
inline real32 pi_over_2(void)    { return 1.57079632679f; } // oeis.org code:A019669
inline real32 pi_over_3(void)    { return 1.04719755120f; } // oeis.org code:A019670
inline real32 pi_over_4(void)    { return 0.78539816340f; } // oeis.org code:A003881
inline real32 pi_over_180(void)  { return 0.01745329252f; } // oeis.org code:A019685
inline real32 pi_under_180(void) { return 57.2957795131f; } // oeis.org code:A072097

// Trignometric Conversions
inline real32 radians(real32 d) { return d*pi_over_180(); }
inline real32 degrees(real32 r) { return r*pi_under_180(); }

#pragma endregion TRIGNOMETRY

#pragma region EULER_ANGLES

void matrix4_to_eulerXYZ(real32* v, const real32* m)
{
 // if z-axis row is close to <+1, 0, 0>
 real32 t1 = std::fabs(1.0f - m[0x8]);
 real32 t2 = std::fabs(1.0f + m[0x8]);
 if(t1 < 1.0e-6f) {
    v[2] = 0.0f;
    v[1] = -pi_over_2();
    v[0] = std::atan2(-m[0x1], -m[0x2]);
   }
 // if z-axis row is close to <-1, 0, 0>
 else if(t2 < 1.0e-6f) {
    v[2] = 0.0f;
    v[1] = +pi_over_2();
    v[0] = std::atan2(m[0x1], m[0x2]);
   }
 else {
    v[1] = -std::asin(m[0x8]);
    real32 div = std::cos(v[1]);
    v[0] = std::atan2(m[0x9]/div, m[0xA]/div);
    v[2] = std::atan2(m[0x4]/div, m[0x0]/div);
   }
}

#pragma endregion EULER_ANGLES

// QUATERNIONS
#pragma region QUATERNIONS

inline real32 qnormalize(real32* q)
{
 real32 norm = std::sqrt(q[0]*q[0] + q[1]*q[1] + q[2]*q[2] + q[3]*q[3]);
 if(norm < 1.0e-7f) return norm;
 real32 scale = 1.0f/norm;
 q[0] *= scale;
 q[1] *= scale;
 q[2] *= scale;
 q[3] *= scale;
 return norm;
}

// returns m = q
inline void quaternion_to_matrix3(real32* m, const real32* q)
{
 // factors
 real32 a2 = q[0] + q[0];
 real32 b2 = q[1] + q[1];
 real32 c2 = q[2] + q[2];
 real32 d2 = q[3] + q[3];

 // factors
 real32 ab2 = q[0] * b2;
 real32 ac2 = q[0] * c2;
 real32 ad2 = q[0] * d2;
 real32 bb2 = q[1] * b2;
 real32 bc2 = q[1] * c2;
 real32 bd2 = q[1] * d2;
 real32 cc2 = q[2] * c2;
 real32 cd2 = q[2] * d2;
 real32 dd2 = q[3] * d2;

 // reinterpret matrix
 real32 one = 1.0f;
 m[0] = one - cc2 - dd2;
 m[1] = bc2 - ad2;
 m[2] = bd2 + ac2;
 m[3] = bc2 + ad2;
 m[4] = one - bb2 - dd2;
 m[5] = cd2 - ab2;
 m[6] = bd2 - ac2;
 m[7] = cd2 + ab2;
 m[8] = one - bb2 - cc2;
}

inline void quaternion_to_matrix4(real32* m, const real32* q)
{
 // factors
 real32 a2 = q[0] + q[0];
 real32 b2 = q[1] + q[1];
 real32 c2 = q[2] + q[2];
 real32 d2 = q[3] + q[3];

 // factors
 real32 ab2 = q[0] * b2;
 real32 ac2 = q[0] * c2;
 real32 ad2 = q[0] * d2;
 real32 bb2 = q[1] * b2;
 real32 bc2 = q[1] * c2;
 real32 bd2 = q[1] * d2;
 real32 cc2 = q[2] * c2;
 real32 cd2 = q[2] * d2;
 real32 dd2 = q[3] * d2;

 // reinterpret matrix
 real32 one = 1.0f;
 m[0x0] = one - cc2 - dd2;
 m[0x1] = bc2 - ad2;
 m[0x2] = bd2 + ac2;
 m[0x3] = 0.0f;
 m[0x4] = bc2 + ad2;
 m[0x5] = one - bb2 - dd2;
 m[0x6] = cd2 - ab2;
 m[0x7] = 0.0f;
 m[0x8] = bd2 - ac2;
 m[0x9] = cd2 + ab2;
 m[0xA] = one - bb2 - cc2;
 m[0xB] = 0.0f;
 m[0xC] = 0.0f;
 m[0xD] = 0.0f;
 m[0xE] = 0.0f;
 m[0xF] = 1.0f;
}

inline void matrix3_to_quaternion(real32* q, const real32* m)
{
 // row 1
 // m[0x0] = 1 - 2*q[2]*q[2] - 2*q[3]*q[3]
 // m[0x1] = 2*q[1]*q[2] - 2*q[0]*q[3]
 // m[0x2] = 2*q[1]*q[3] + 2*q[0]*q[2]

 // m[0x1]*q[2] = 2*q[1]*q[2]*q[2] - 2*q[0]*q[2]*q[3]
 // m[0x2]*q[3] = 2*q[1]*q[3]*q[3] + 2*q[0]*q[2]*q[3]

 // m[0x1]*q[2] + m[0x2]*q[3] = 2*q[1]*q[2]*q[2] + 2*q[1]*q[3]*q[3]
 // m[0x1]*q[2] + m[0x2]*q[3] = q[1]*(2*q[2]*q[2] + 2*q[3]*q[3])
 // m[0x1]*q[2] + m[0x2]*q[3] = q[1]*(1 - m[0x0])

 // row 1 result
 // (m[0x0] - 1)*q[1] + m[0x1]*q[2] + m[0x2]*q[3] = 0

 // row 2
 // m[0x3] = 2*q[1]*q[2] + 2*q[0]*q[3]
 // m[0x4] = 1 - 2*q[1]*q[1] - 2*q[3]*q[3]
 // m[0x5] = 2*q[2]*q[3] - 2*q[0]*q[1]

 // q[1]*m[0x3] = 2*q[1]*q[1]*q[2] + 2*q[0]*q[1]*q[3]
 // q[3]*m[0x5] = 2*q[2]*q[3]*q[3] - 2*q[0]*q[1]*q[3]

 // q[1]*m[0x3] + q[3]*m[0x5] = 2*q[1]*q[1]*q[2] + 2*q[2]*q[3]*q[3]

 // q[1]*m[0x3] + q[3]*m[0x5] = q[2]*(2*q[1]*q[1] + 2*q[3]*q[3])
 // 1 - m[0x4] = 2*q[1]*q[1] + 2*q[3]*q[3]
 // q[1]*m[0x3] + q[3]*m[0x5] = q[2]*(1 - m[0x4])

 // row 2 result
 // m[0x3]*q[1] + (m[0x4] - 1)*q[2] + m[0x5]*q[3] = 0

 // results three equations - three unknowns
 // (m[0x0] - 1)*q[1] + m[0x1]*q[2] + m[0x2]*q[3] = 0
 // m[0x3]*q[1] + (m[0x4] - 1)*q[2] + m[0x5]*q[3] = 0
 // m[0x6]*q[1] + m[0x7]*q[2] + (m[0x8] - 1)*q[3] = 0

 //  m[0x3]*m[0x6]*q[1] + (m[0x4] - 1)*m[0x6]*q[2] + m[0x5]*m[0x6]*q[3] = 0
 // -m[0x3]*m[0x6]*q[1] - m[0x3]*m[0x7]*q[2] - m[0x3]*(m[0x8] - 1)*q[3] = 0
 // -----------------------------------------------------------------------
 // [(m[0x4] - 1)*m[0x6] - m[0x3]*m[0x7]]*q[2] + [m[0x5]*m[0x6] - m[0x3]*(m[0x8] - 1)]*q[3] = 0

}

inline void matrix4_to_quaternion(real32* q, const real32* m)
{
}

#pragma endregion QUATERNIONS

// INTERPOLATION
#pragma region INTERPOLATION

inline void lerp3D(real32* v, const real32* A, const real32* B, real32 t)
{
 v[0] = A[0] + t*(B[0] - A[0]);
 v[1] = A[1] + t*(B[1] - A[1]);
 v[2] = A[2] + t*(B[2] - A[2]);
}

inline void qslerp(real32* v, const real32* A, const real32* B, real32 t)
{
 real32 scale = A[0]*B[0] + A[1]*B[1] + A[2]*B[2] + A[3]*B[3];
 if(scale < 0.0f)
   {
    // if A and B are nearly the same
    if(scale < -0.9999f) {
       v[0] = A[0];
       v[1] = A[1];
       v[2] = A[2];
       v[3] = A[3];
       return;
      }

    real32 r1 = std::acos(-scale);
    real32 r2 = r1*t;
    real32 r3 = r1 - r2;
    real32 denom = sin(r1);
    real32 s1 = std::sin(r3)/denom;
    real32 s2 = std::sin(r2)/denom;

    v[0] = A[0]*s1 - B[0]*s2;
    v[1] = A[1]*s1 - B[1]*s2;
    v[2] = A[2]*s1 - B[2]*s2;
    v[3] = A[3]*s1 - B[3]*s2;
   }
 else
   {
    // if A and B are nearly the same
    if(scale > 0.9999f) {
       v[0] = A[0];
       v[1] = A[1];
       v[2] = A[2];
       v[3] = A[3];
       return;
      }

    real32 r1 = std::acos(scale);
    real32 r2 = r1*t;
    real32 r3 = r1 - r2;
    real32 denom = sin(r1);
    real32 s1 = std::sin(r3)/denom;
    real32 s2 = std::sin(r2)/denom;

    v[0] = A[0]*s1 + B[0]*s2;
    v[1] = A[1]*s1 + B[1]*s2;
    v[2] = A[2]*s1 + B[2]*s2;
    v[3] = A[3]*s1 + B[3]*s2;
   }
}

#pragma endregion INTERPOLATION

#endif
