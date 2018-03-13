#include<iostream>
#include<array>

struct AABB_halfdim {
 float center[3];
 float widths[3];
};

struct vector3D {
 float v[3];
 float& operator [](size_t i) { return v[i]; }
 const float& operator [](size_t i)const { return v[i]; }
};

class octree {
 private :
  struct node {
   AABB_halfdim volume;
   std::array<node, 8> children;
  };
  node root;
 public :
  void construct(const vector3D* data, size_t n);
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

void octree::construct(const vector3D* data, size_t n)
{
 if(!data || !n) return;

 // compute min-max bounds
 float min_b[3] = { data[0][0], data[0][1], data[0][2] };
 float max_b[3] = { data[0][0], data[0][1], data[0][2] };
 for(size_t i = 1; i < n; i++) {
     // min bounds
     if(data[i][0] < min_b[0]) min_b[0] = data[i][0];
     if(data[i][1] < min_b[1]) min_b[1] = data[i][1];
     if(data[i][2] < min_b[2]) min_b[2] = data[i][2];
     // max bounds
     if(max_b[0] < data[i][0]) max_b[0] = data[i][0];
     if(max_b[1] < data[i][1]) max_b[1] = data[i][1];
     if(max_b[2] < data[i][2]) max_b[2] = data[i][2];
    }

 // define root node
 root.volume.center[0] = (max_b[0] - min_b[0])/2.0f;
 root.volume.center[1] = (max_b[1] - min_b[1])/2.0f;
 root.volume.center[2] = (max_b[2] - min_b[2])/2.0f;
 root.volume.widths[0] = max_b[0] - root.volume.center[0];
 root.volume.widths[1] = max_b[1] - root.volume.center[1];
 root.volume.widths[2] = max_b[2] - root.volume.center[2];
}

void octree::clear()
{
}