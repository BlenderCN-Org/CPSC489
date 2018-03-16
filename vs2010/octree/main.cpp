#include<iostream>
#include<fstream>
#include<cmath>
#include<memory>

struct AABB_halfdim {
 float center[3];
 float widths[3];
};

struct AABB_minmax {
 float a[3];
 float b[3];
};

inline std::ostream& operator <<(std::ostream& os, const AABB_halfdim& aabb)
{
 os << "AABB[0]: <" << (aabb.center[0] - aabb.widths[0]) <<
               ", " << (aabb.center[1] - aabb.widths[1]) <<
               ", " << (aabb.center[2] - aabb.widths[2]) << ">" << std::endl;
 os << "AABB[1]: <" << (aabb.center[0] + aabb.widths[0]) <<
               ", " << (aabb.center[1] - aabb.widths[1]) <<
               ", " << (aabb.center[2] - aabb.widths[2]) << ">" << std::endl;
 os << "AABB[2]: <" << (aabb.center[0] - aabb.widths[0]) <<
               ", " << (aabb.center[1] + aabb.widths[1]) <<
               ", " << (aabb.center[2] - aabb.widths[2]) << ">" << std::endl;
 os << "AABB[3]: <" << (aabb.center[0] + aabb.widths[0]) <<
               ", " << (aabb.center[1] + aabb.widths[1]) <<
               ", " << (aabb.center[2] - aabb.widths[2]) << ">" << std::endl;
 os << "AABB[4]: <" << (aabb.center[0] - aabb.widths[0]) <<
               ", " << (aabb.center[1] - aabb.widths[1]) <<
               ", " << (aabb.center[2] + aabb.widths[2]) << ">" << std::endl;
 os << "AABB[5]: <" << (aabb.center[0] + aabb.widths[0]) <<
               ", " << (aabb.center[1] - aabb.widths[1]) <<
               ", " << (aabb.center[2] + aabb.widths[2]) << ">" << std::endl;
 os << "AABB[6]: <" << (aabb.center[0] - aabb.widths[0]) <<
               ", " << (aabb.center[1] + aabb.widths[1]) <<
               ", " << (aabb.center[2] + aabb.widths[2]) << ">" << std::endl;
 os << "AABB[7]: <" << (aabb.center[0] + aabb.widths[0]) <<
               ", " << (aabb.center[1] + aabb.widths[1]) <<
               ", " << (aabb.center[2] + aabb.widths[2]) << ">" << std::endl;
 return os;
}

struct vector3D {
 float v[3];
 float& operator [](size_t i) { return v[i]; }
 const float& operator [](size_t i)const { return v[i]; }
 vector3D() {}
 vector3D(float x, float y, float z) {
  v[0] = x;
  v[1] = y;
  v[2] = z;
 }
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
  void construct(const vector3D* verts, size_t n_verts, const unsigned int* faces, size_t n_indices);
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

void SplitTest(void)
{
 // box to split
 AABB_minmax mmb;
 mmb.a[0] = 1.0f; mmb.a[1] = 2.0f; mmb.a[2] = 3.0f;
 mmb.b[0] = 5.0f; mmb.b[1] = 4.0f; mmb.b[2] = 6.0f;

 // compute median
 float x_mid = (mmb.a[0] + mmb.b[0])/2.0f;
 float y_mid = (mmb.a[1] + mmb.b[1])/2.0f;
 float z_mid = (mmb.a[2] + mmb.b[2])/2.0f;

 // for convenience
 float x_min = mmb.a[0]; float x_max = mmb.b[0];
 float y_min = mmb.a[1]; float y_max = mmb.b[1];
 float z_min = mmb.a[2]; float z_max = mmb.b[2];

 // split[0]
 AABB_minmax split[8];
 split[0].a[0] = x_mid; split[0].a[1] = y_min; split[0].a[2] = z_mid;
 split[0].b[0] = x_max; split[0].b[1] = y_mid; split[0].b[2] = z_max;
 // split[1]
 split[1].a[0] = x_min; split[1].a[1] = y_min; split[1].a[2] = z_mid;
 split[1].b[0] = x_mid; split[1].b[1] = y_mid; split[1].b[2] = z_max;
 // split[2]
 split[2].a[0] = x_min; split[2].a[1] = y_mid; split[2].a[2] = z_mid;
 split[2].b[0] = x_mid; split[2].b[1] = y_max; split[2].b[2] = z_max;
 // split[3]
 split[3].a[0] = x_mid; split[3].a[1] = y_mid; split[3].a[2] = z_mid;
 split[3].b[0] = x_max; split[3].b[1] = y_max; split[3].b[2] = z_max;

 // lower-half (only z values change)
 // split[4]
 split[4].a[0] = x_mid; split[4].a[1] = y_min; split[4].a[2] = z_min;
 split[4].b[0] = x_max; split[4].b[1] = y_mid; split[4].b[2] = z_mid;
 // split[5]
 split[5].a[0] = x_min; split[5].a[1] = y_min; split[5].a[2] = z_min;
 split[5].b[0] = x_mid; split[5].b[1] = y_mid; split[5].b[2] = z_mid;
 // split[6]
 split[6].a[0] = x_min; split[6].a[1] = y_mid; split[6].a[2] = z_min;
 split[6].b[0] = x_mid; split[6].b[1] = y_max; split[6].b[2] = z_mid;
 // split[7]
 split[7].a[0] = x_mid; split[7].a[1] = y_mid; split[7].a[2] = z_min;
 split[7].b[0] = x_max; split[7].b[1] = y_max; split[7].b[2] = z_mid;

 using namespace std;
 ofstream ofile("octrees_split.obj");
 ofile << "o octrees_split.obj" << endl;

 // boxes
 int base = 0;
 for(int i = 0; i < 8; i++) {
     ofile << "v " << split[i].a[0] << " " << split[i].a[1] << " " << -split[i].a[2] << endl; // L-side
     ofile << "v " << split[i].b[0] << " " << split[i].a[1] << " " << -split[i].a[2] << endl; // L-side
     ofile << "v " << split[i].b[0] << " " << split[i].a[1] << " " << -split[i].b[2] << endl; // L-side
     ofile << "v " << split[i].a[0] << " " << split[i].a[1] << " " << -split[i].b[2] << endl; // L-side
     ofile << "v " << split[i].a[0] << " " << split[i].b[1] << " " << -split[i].a[2] << endl; // R-side
     ofile << "v " << split[i].b[0] << " " << split[i].b[1] << " " << -split[i].a[2] << endl; // R-side
     ofile << "v " << split[i].b[0] << " " << split[i].b[1] << " " << -split[i].b[2] << endl; // R-side
     ofile << "v " << split[i].a[0] << " " << split[i].b[1] << " " << -split[i].b[2] << endl; // R-side
     ofile << "f " << (base + 1) << " " << (base + 2) << endl; // L-side
     ofile << "f " << (base + 2) << " " << (base + 3) << endl; // L-side
     ofile << "f " << (base + 3) << " " << (base + 4) << endl; // L-side
     ofile << "f " << (base + 4) << " " << (base + 1) << endl; // L-side
     ofile << "f " << (base + 5) << " " << (base + 6) << endl; // R-side
     ofile << "f " << (base + 6) << " " << (base + 7) << endl; // R-side
     ofile << "f " << (base + 7) << " " << (base + 8) << endl; // R-side
     ofile << "f " << (base + 8) << " " << (base + 5) << endl; // R-side
     ofile << "f " << (base + 1) << " " << (base + 5) << endl; // L/R-side
     ofile << "f " << (base + 2) << " " << (base + 6) << endl; // L/R-side
     ofile << "f " << (base + 3) << " " << (base + 7) << endl; // L/R-side
     ofile << "f " << (base + 4) << " " << (base + 8) << endl; // L/R-side
     ofile << endl;
     base += 8;
    }

 // min-max diagonals
 for(int i = 0; i < 8; i++) {
     ofile << "v " << split[i].a[0] << " " << split[i].a[1] << " " << -split[i].a[2] << endl;
     ofile << "v " << split[i].b[0] << " " << split[i].b[1] << " " << -split[i].b[2] << endl;
     ofile << "f " << (base + 1) << " " << (base + 2) << endl;
     base += 2;
    }
}

int main()
{
 SplitTest();

 // see sample.lwo
 vector3D points[24] = {
  vector3D(1.4f, 2.3f, 7.0f), vector3D(1.7f, 2.5f, 5.8f), vector3D(2.4f, 2.0f, 5.3f), // 0 1 2
  vector3D(2.1f, 1.2f, 2.8f), vector3D(1.3f, 1.6f, 2.1f), vector3D(1.8f, 1.4f, 1.6f), // 3 4 5
  vector3D(4.1f, 1.1f, 7.5f), vector3D(4.3f, 1.3f, 7.1f), vector3D(4.7f, 1.3f, 7.7f), // 6 7 8
  vector3D(2.7f, 1.5f, 4.6f), vector3D(3.1f, 1.0f, 4.4f), vector3D(3.2f, 1.4f, 4.2f), // 9 10 11
  vector3D(5.1f, 1.4f, 5.6f), vector3D(6.0f, 1.1f, 5.6f), vector3D(5.6f, 1.2f, 4.3f), // 12 13 14
  vector3D(8.0f, 0.1f, 4.8f), vector3D(7.8f, 0.2f, 4.2f), vector3D(8.4f, 0.5f, 3.7f), // 15 16 17
  vector3D(3.5f, 1.1f, 3.4f), vector3D(3.0f, 0.6f, 1.3f), vector3D(4.3f, 0.5f, 2.2f), // 18 19 20
  vector3D(6.9f, 0.9f, 3.9f), vector3D(7.3f, 0.5f, 3.5f), vector3D(6.3f, 0.9f, 3.4f), // 21 22 23
 };

 unsigned int facelist[24] = {
   0,  1,  2,
   3,  4,  5,
   6,  7,  8,
   9, 10, 11,
  12, 13, 14,
  15, 16, 17,
  18, 19, 20,
  21, 22, 23,
 };

 octree tree;
 tree.construct(points, 24, facelist, 24);
 
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

void octree::construct(const vector3D* verts, size_t n_verts, const unsigned int* faces, size_t n_indices)
{
 // binning example
 // dv = (max_v - min_v)/n_bin = (2*box_w)/n_bin
 // bin       0     1     2     3     4     5     6     7
 // range 4.0 - 4.5 - 5.0 - 5.5 - 6.0 - 6.5 - 7.0 - 7.5 - 8.0 (split_v)
 // L_cnt     x     x     x     x     x     x     x     x
 // R_cnt     x     x     x     x     x     x     x     x 

 if(!verts || !n_verts) return;
 if(!faces || !n_indices) return;

 // compute number of faces
 size_t n_faces = n_indices/3;
 if(!n_faces) return;

 // debug
 using namespace std;
 bool debug = true;
 ofstream ofile;
 int vb_index = 0;
 if(debug) {
    ofile.open("debug.obj");
    ofile << "o debug.obj" << endl;
   }

 // compute centroids
 std::unique_ptr<vector3D[]> centroids(new vector3D[n_faces]);
 size_t a = 0;
 centroids[0] = triangle_centroid(verts[faces[a++]], verts[faces[a++]], verts[faces[a++]]);
 for(size_t i = 1; i < n_faces; i++) centroids[i] = triangle_centroid(verts[faces[a++]], verts[faces[a++]], verts[faces[a++]]);

 // output centroids
 if(debug) {
    for(size_t i = 0; i < n_faces; i++)
        ofile << "v " << centroids[i][0] << " " << centroids[i][1] << " " << -centroids[i][2] << endl;        
    vb_index += n_faces;
   }

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
 root->volume.center[0] = (max_b[0] + min_b[0])/2.0f;
 root->volume.center[1] = (max_b[1] + min_b[1])/2.0f;
 root->volume.center[2] = (max_b[2] + min_b[2])/2.0f;
 root->volume.widths[0] = max_b[0] - root->volume.center[0];
 root->volume.widths[1] = max_b[1] - root->volume.center[1];
 root->volume.widths[2] = max_b[2] - root->volume.center[2];
 std::cout << "root AABB = " << root->volume << std::endl;

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

     std::cout << "best_d = " << std::endl;
     std::cout << best_d[0] << ", " << best_d[1] << ", " << best_d[2] << std::endl;
     std::cout << "best_i = " << std::endl;
     std::cout << best_i[0] << ", " << best_i[1] << ", " << best_i[2] << std::endl;

     // now that we have our split point, we can partition
     // the AABB into eight parts
     float split_point[3] = {
      split_v[0][best_i[0] + 1],
      split_v[1][best_i[1] + 1],
      split_v[2][best_i[2] + 1],
     };

     std::cout << "split point = " << std::endl;
     std::cout << split_point[0] << ", " << split_point[1] << ", " << split_point[2] << std::endl;

     //
     if(debug) {
        // split X-axis
        float X[4][3] = {
         {
          split_point[0],                                  // split x
          curr->volume.center[1] + curr->volume.widths[1], // +y
          curr->volume.center[2] - curr->volume.widths[2], // -z
         },
         {
          split_point[0],                                  // split x
          curr->volume.center[1] - curr->volume.widths[1], // -y
          curr->volume.center[2] - curr->volume.widths[2], // -z
         },
         {
          split_point[0],                                  // split x
          curr->volume.center[1] - curr->volume.widths[1], // -y
          curr->volume.center[2] + curr->volume.widths[2], // +z
         },
         {
          split_point[0],                                  // split x
          curr->volume.center[1] + curr->volume.widths[1], // +y
          curr->volume.center[2] + curr->volume.widths[2], // +z
         },
        };
        ofile << "v " << X[0][0] << " " << X[0][1] << " " << -X[0][2] << endl;
        ofile << "v " << X[1][0] << " " << X[1][1] << " " << -X[1][2] << endl;
        ofile << "v " << X[2][0] << " " << X[2][1] << " " << -X[2][2] << endl;
        ofile << "v " << X[3][0] << " " << X[3][1] << " " << -X[3][2] << endl;
        int start = vb_index + 1;
        ofile << "f " << (start + 0) << " " << (start + 1) << " " << (start + 2) << " " << (start + 3) << endl;
        vb_index += 4;

        // split Z-axis
        float p[4][3] = {
         {
          curr->volume.center[0] + curr->volume.widths[0], // +x
          curr->volume.center[1] - curr->volume.widths[1], // -y
          split_point[2]                                   // split z
         },
         {
          curr->volume.center[0] - curr->volume.widths[0], // -x
          curr->volume.center[1] - curr->volume.widths[1], // -y
          split_point[2]                                   // split z
         },
         {
          curr->volume.center[0] - curr->volume.widths[0], // -x
          curr->volume.center[1] + curr->volume.widths[1], // +y
          split_point[2]                                   // split z
         },
         {
          curr->volume.center[0] + curr->volume.widths[0], // +x
          curr->volume.center[1] + curr->volume.widths[1], // +y
          split_point[2]                                   // split z
         },
        };
        ofile << "v " << p[0][0] << " " << p[0][1] << " " << -p[0][2] << endl;
        ofile << "v " << p[1][0] << " " << p[1][1] << " " << -p[1][2] << endl;
        ofile << "v " << p[2][0] << " " << p[2][1] << " " << -p[2][2] << endl;
        ofile << "v " << p[3][0] << " " << p[3][1] << " " << -p[3][2] << endl;
        start = vb_index + 1;
        ofile << "f " << (start + 0) << " " << (start + 1) << " " << (start + 2) << " " << (start + 3) << endl;
        vb_index += 4;
       }
    }
}

void octree::clear()
{
}