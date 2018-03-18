#include<iostream>
using namespace std;

struct line3D {
 float p1[3];
 float p2[3];
};

struct AABB_minmax {
 float a[3];
 float b[3];
};

bool intersect(const AABB_minmax& aabb, const line3D& line);

int main()
{
 AABB_minmax box;
 box.a[0] = 0.0f;
 box.a[1] = 0.0f;
 box.a[2] = 0.0f;
 box.b[0] = 8.0f;
 box.b[1] = 4.0f;
 box.b[2] = 6.0f;

 //#define CASE_1 // TRUE
 //#define CASE_2 // FALSE
 #define CASE_3 // TRUE

 #if defined CASE_1
 line3D line;
 line.p1[0] = 1.0f;
 line.p1[1] = 7.0f;
 line.p1[2] = 4.0f;
 line.p2[0] = 0.0f;
 line.p2[1] = 2.0f;
 line.p2[2] = 4.0f;
 #elif defined CASE_2
 line3D line;
 line.p1[0] = 1.0f;
 line.p1[1] = 7.0f;
 line.p1[2] = 4.0f;
 line.p2[0] = -0.7f;
 line.p2[1] = 2.0f;
 line.p2[2] = 4.0f;
 #elif defined CASE_3
 line3D line;
 line.p1[0] = 1.0f;
 line.p1[1] = 7.0f;
 line.p1[2] = 4.0f;
 line.p2[0] = -0.64839f;
 line.p2[1] = 2.0f;
 line.p2[2] = 4.0f;
 #endif

 bool result = intersect(box, line);
 if(result) cout << "Line intersects box." << endl;
 else cout << "Line does not intersect box." << endl;
 cout << endl;

 return -1;
}

bool intersect(const AABB_minmax& aabb, const line3D& line)
{
 // translate line to AABB origin
 float center[3] = {
  (aabb.a[0] + aabb.b[0])/2.0f,
  (aabb.a[1] + aabb.b[1])/2.0f,
  (aabb.a[2] + aabb.b[2])/2.0f
 };
 float p1[3] = {
  line.p1[0] - center[0],
  line.p1[1] - center[1],
  line.p1[2] - center[2]
 };
 float p2[3] = {
  line.p2[0] - center[0],
  line.p2[1] - center[1],
  line.p2[2] - center[2]
 };

 // AABB half dimensions
 float h[3] = {
  (aabb.b[0] - aabb.a[0])/2.0f,
  (aabb.b[1] - aabb.a[1])/2.0f,
  (aabb.b[2] - aabb.a[2])/2.0f,
 };

 // center of line segment translated to AABB origin
 float c[3] = {
  (p1[0] + p2[0])/2.0f,
  (p1[1] + p2[1])/2.0f,
  (p1[2] + p2[2])/2.0f
 };

 // half-vector and positive half-vector
 // w = p2 - c
 float w[3] = {
  p2[0] - c[0],
  p2[1] - c[1],
  p2[2] - c[2],
 };
 float v[3] = {
  std::fabs(w[0]),
  std::fabs(w[1]),
  std::fabs(w[2]),
 };

 // axis tests
 float epsilon = 1.0e-6f;
 if((v[0] + h[0] + epsilon) < std::fabs(c[0])) return false;
 if((v[1] + h[1] + epsilon) < std::fabs(c[1])) return false;
 if((v[2] + h[2] + epsilon) < std::fabs(c[2])) return false;

 // cross product axis tests
 if((h[1]*v[2] + h[2]*v[1] + epsilon) < std::fabs(c[1]*w[2] - c[2]*w[1])) return false;
 if((h[0]*v[2] + h[2]*v[0] + epsilon) < std::fabs(c[0]*w[2] - c[2]*w[0])) return false;
 if((h[0]*v[1] + h[1]*v[0] + epsilon) < std::fabs(c[0]*w[1] - c[1]*w[0])) return false;

 return true;
}