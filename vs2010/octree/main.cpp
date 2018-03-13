#include<iostream>
#include<array>

struct AABB_halfdim {
 float center[3];
 float widths[3];
};

struct vector3D {
 float v[3];
};

class octree {
 private :
  struct node {
   AABB_halfdim volume;
   std::array<node, 8> children;
  };
  node root;
 public :
  void construct(const vector3D* data);
  void clear();
 public :
  octree& operator =(const octree& other);
  octree& operator =(octree&& other);
 public :
  octree();
  octree(const octree& other);
  octree(octree&& other);
 ~octree();
};

int main()
{
 return -1;
}

octree::octree()
{
}

octree::octree(const octree& other)
{
}

octree::octree(octree&& other)
{
}

octree::~octree()
{
}

octree& octree::operator =(const octree& other)
{
 if(this == &other) return *this;
 return *this;
}

octree& octree::operator =(octree&& other)
{
 if(this == &other) return *this;
 return *this;
}

void octree::construct(const vector3D* data)
{
}

void octree::clear()
{
}