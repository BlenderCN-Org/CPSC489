#ifndef __CPSC489_MATH_H
#define __CPSC489_MATH_H

//
// SPECIAL FUNCTIONS
//

inline real32 inv(real32 x)
{
 return 1.0f/x;
}

//
// RULE OF SARRUS
//

inline real32 sarrus_2x2(real32 m11, real32 m12, real32 m21, real32 m22)
{
 return m11*m22 - m12*m21;
}

inline real32 sarrus_2x2(const real32* m)
{
 return m[0]*m[3] - m[1]*m[2];
}

//
// SPECIAL_NUMBERS
//

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

//
// TRIGNOMETRY
//

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

#endif
