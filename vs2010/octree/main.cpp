#include<iostream>
#include<cmath>
#include<memory>

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
  static const int n_bin = 8;
  static const int n_split = n_bin + 1;
 private :
  struct node {
   AABB_halfdim volume;
   std::unique_ptr<node> children[8];
  };
  std::unique_ptr<node> root;
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
 // binning example
 // dv = (max_v - min_v)/n_bin = (2*box_w)/n_bin
 // bin       0     1     2     3     4     5     6     7
 // range 4.0 - 4.5 - 5.0 - 5.5 - 6.0 - 6.5 - 7.0 - 7.5 - 8.0 (split_v)
 // L_cnt     x     x     x     x     x     x     x     x
 // R_cnt     x     x     x     x     x     x     x     x 

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
 root = std::unique_ptr<node>(new node);
 root->volume.center[0] = (max_b[0] - min_b[0])/2.0f;
 root->volume.center[1] = (max_b[1] - min_b[1])/2.0f;
 root->volume.center[2] = (max_b[2] - min_b[2])/2.0f;
 root->volume.widths[0] = max_b[0] - root->volume.center[0];
 root->volume.widths[1] = max_b[1] - root->volume.center[1];
 root->volume.widths[2] = max_b[2] - root->volume.center[2];

 // loop to split
 node* curr = root.get();
 // for(;;)
    {
     // AABB properties
     const float min_x = (curr->volume.center[0] - curr->volume.widths[0]);
     const float min_y = (curr->volume.center[1] - curr->volume.widths[1]);
     const float min_z = (curr->volume.center[2] - curr->volume.widths[2]);
     const float dx = (2.0f*curr->volume.widths[0])/n_bin;
     const float dy = (2.0f*curr->volume.widths[1])/n_bin;
     const float dz = (2.0f*curr->volume.widths[2])/n_bin;

     // define split intervals
     float split_v[3][n_split];
     for(int i = 0; i < n_split; i++) {
         split_v[0][i] = min_x + i*dx;
         split_v[1][i] = min_y + i*dy;
         split_v[2][i] = min_z + i*dz;
        }

     // keep track of number of faces in each bin
     int bin[3][n_bin];
     for(size_t i = 0; i < n_bin; i++) bin[0][i] = bin[1][i] = bin[2][i] = 0;

     // partition face centroids into bins
     for(size_t i = 0; i < n_faces; i++)
        {
         // split x-axis
         for(int j = 0; j < n_bin; j++) {
             float a = split_v[0][j];
             float b = split_v[0][j + 1];
             if(centroids[i][0] >= a && centroids[i][0] <= b) {
                bin[0][j]++;
                break;
               }
            }
         // split y-axis
         for(int j = 0; j < n_bin; j++) {
             float a = split_v[1][j];
             float b = split_v[1][j + 1];
             if(centroids[i][1] >= a && centroids[i][1] <= b) {
                bin[1][j]++;
                break;
               }
            }
         // split z-axis
         for(int j = 0; j < n_bin; j++) {
             float a = split_v[2][j];
             float b = split_v[2][j + 1];
             if(centroids[i][2] >= a && centroids[i][2] <= b) {
                bin[2][j]++;
                break;
               }
            }
        }

     // determine best axes split
     int L_count[3] = { bin[0][0], bin[1][0], bin[2][0] };
     int R_count[3] = { 
      n_faces - bin[0][0],
      n_faces - bin[1][0],
      n_faces - bin[2][0],
     };
     int best_d[3] = {
      std::abs(R_count[0] - L_count[0]),
      std::abs(R_count[1] - L_count[1]),
      std::abs(R_count[2] - L_count[2]),
     };
     int best_i[3] = { 0, 0, 0 };
     for(int i = 1; i < n_bin; i++) {
         for(int j = 0; j < 3; j++) {
             L_count[j] += bin[j][i];
             R_count[j] -= bin[j][i];
             int difference = std::abs(R_count[j] - L_count[j]);
             if(difference < best_d[j]) {
                best_d[j] = difference;
                best_i[j] = i;
               }
            }
        }

     // now that we have our split point, we can partition
     // the AABB into eight parts
     float split_point[3] = {
      split_v[0][best_i[0] + 1],
      split_v[1][best_i[1] + 1],
      split_v[2][best_i[2] + 1],
     };
    }
}

void octree::clear()
{
}