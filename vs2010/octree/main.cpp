#include<iostream>
#include<array>

struct AABB_halfdim {
 float center[4];
 float widths[4];
};

class octree {
 private :
  struct node {
   AABB_halfdim volume;
   std::array<node, 8> children;
  };
  node root;
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