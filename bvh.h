#ifndef __CS_BVH_H
#define __CS_BVH_H

#include "vector3.h"
#include "aabb.h"
#include "sphere3.h"

struct PointLinearCollisionTest {
 // data
 vector3D point; // point to test
 vector3D D;     // direction point is moving
 float t1;       // time interval
 float t2;       // time interval
 // results
 bool collide;   // is there a collision
 float t;        // time interval
};

struct SphereLinearCollisionTest {
 // data
 sphere3D S;     // sphere to test
 vector3D D;     // direction point is moving
 float t1;       // time interval
 float t2;       // time interval
 // results
 bool collide;   // is there a collision
 float t;        // time interval
};

class BVH {
 private :
  static const int n_bins = 8;
  static const int n_part = n_bins - 1;
 private :
  struct AABB_node {
   AABB_minmax aabb;
   unsigned int params[2];
  };
  std::vector<AABB_node> tree;
 public :
  void construct(const vector3D* verts, size_t n_verts, unsigned int* faces, size_t n_indices);
  void clear();
 public :
  void collide(PointLinearCollisionTest& info);
  void collide(SphereLinearCollisionTest& info);
 public :
  BVH& operator =(const BVH& other) = delete;
  BVH& operator =(BVH&& other);
 public :
  BVH();
  BVH(const BVH& other) = delete;
  BVH(BVH&& other);
 ~BVH();
};

inline BVH::BVH()
{
}

inline BVH::BVH(BVH&& other)
{
 this->tree = std::move(other.tree);
}

inline BVH::~BVH()
{
}

inline BVH& BVH::operator =(BVH&& other)
{
 if(this == &other) return *this;
 this->tree = std::move(other.tree);
 return *this;
}

inline void BVH::clear()
{
 this->tree.clear();
}


#endif
