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

inline vector3D triangle_centroid(const vector3D& v1, const vector3D& v2, const vector3D& v3)
{
 vector3D rv;
 rv[0] = (v1[0] + v2[0] + v3[0])*0.333333333333f;
 rv[1] = (v1[1] + v2[1] + v3[1])*0.333333333333f;
 rv[2] = (v1[2] + v2[2] + v3[2])*0.333333333333f;
 return rv;
}

class octree {
 private :
  struct node {
   AABB_halfdim volume;
   std::array<node, 8> children;
  };
  node root;
 public :
  void construct(const vector3D* verts, size_t n_verts, const unsigned int* faces, size_t n_faces);
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

void octree::construct(const vector3D* verts, size_t n_verts, const unsigned int* faces, size_t n_faces)
{
 if(!verts || !n_verts) return;
 if(!faces || !n_faces) return;

 // compute centroids
 std::unique_ptr<vector3D[]> centroids(new vector3D[n_faces]);
 size_t a = faces[0];
 size_t b = faces[1];
 size_t c = faces[2];
 centroids[0] = triangle_centroid(verts[a++], verts[b++], verts[c++]);
 for(size_t i = 1; i < n_faces; i++) centroids[i] = triangle_centroid(verts[a++], verts[b++], verts[c++]);

 // compute min-max bounds
 float min_b[3] = { verts[0][0], verts[0][1], verts[0][2] };
 float max_b[3] = { verts[0][0], verts[0][1], verts[0][2] };
 for(size_t i = 1; i < n_verts; i++) {
     // min bounds
     if(verts[i][0] < min_b[0]) min_b[0] = verts[i][0];
     if(verts[i][1] < min_b[1]) min_b[1] = verts[i][1];
     if(verts[i][2] < min_b[2]) min_b[2] = verts[i][2];
     // max bounds
     if(max_b[0] < verts[i][0]) max_b[0] = verts[i][0];
     if(max_b[1] < verts[i][1]) max_b[1] = verts[i][1];
     if(max_b[2] < verts[i][2]) max_b[2] = verts[i][2];
    }

 // define root node
 root.volume.center[0] = (max_b[0] - min_b[0])/2.0f;
 root.volume.center[1] = (max_b[1] - min_b[1])/2.0f;
 root.volume.center[2] = (max_b[2] - min_b[2])/2.0f;
 root.volume.widths[0] = max_b[0] - root.volume.center[0];
 root.volume.widths[1] = max_b[1] - root.volume.center[1];
 root.volume.widths[2] = max_b[2] - root.volume.center[2];

 // loop to split
 const int n_split = 8;
 // for(;;)
    {
     // AABB properties
     const float min_x = (root.volume.center[0] - root.volume.widths[0]);
     const float min_y = (root.volume.center[1] - root.volume.widths[1]);
     const float dx = (2.0f*root.volume.widths[0])/(n_split + 2.0f);
     const float dy = (2.0f*root.volume.widths[1])/(n_split + 2.0f);

     // split intervals
     float split_x[n_split];
     float split_y[n_split];
     for(int i = 0; i < n_split; i++) {
         split_x[i] = min_x + i*dx;
         split_y[i] = min_y + i*dy;
        }

     size_t bin_x[n_split + 1];
     size_t bin_y[n_split + 1];
    }
}

void octree::clear()
{
}