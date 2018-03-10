#include<iostream>

// old way of aligning 
__declspec(align(16)) struct AABB_halfdim {
 float center[4];
 float widths[4];
};

inline bool point_in_AABB(const float* v, const AABB_halfdim& aabb)
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
 std::cout << point_in_AABB(v, aabb) << std::endl;
 return 0;
}