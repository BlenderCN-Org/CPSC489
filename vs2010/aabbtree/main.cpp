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

inline void StreamToOBJ(std::ostream& os, const AABB_minmax& box, int& base)
{
 os << "v " << box.a[0] << " " << box.a[1] << " " << -box.a[2] << std::endl; // L-side
 os << "v " << box.b[0] << " " << box.a[1] << " " << -box.a[2] << std::endl; // L-side
 os << "v " << box.b[0] << " " << box.a[1] << " " << -box.b[2] << std::endl; // L-side
 os << "v " << box.a[0] << " " << box.a[1] << " " << -box.b[2] << std::endl; // L-side
 os << "v " << box.a[0] << " " << box.b[1] << " " << -box.a[2] << std::endl; // R-side
 os << "v " << box.b[0] << " " << box.b[1] << " " << -box.a[2] << std::endl; // R-side
 os << "v " << box.b[0] << " " << box.b[1] << " " << -box.b[2] << std::endl; // R-side
 os << "v " << box.a[0] << " " << box.b[1] << " " << -box.b[2] << std::endl; // R-side
 os << "f " << (base + 1) << " " << (base + 2) << std::endl; // L-side
 os << "f " << (base + 2) << " " << (base + 3) << std::endl; // L-side
 os << "f " << (base + 3) << " " << (base + 4) << std::endl; // L-side
 os << "f " << (base + 4) << " " << (base + 1) << std::endl; // L-side
 os << "f " << (base + 5) << " " << (base + 6) << std::endl; // R-side
 os << "f " << (base + 6) << " " << (base + 7) << std::endl; // R-side
 os << "f " << (base + 7) << " " << (base + 8) << std::endl; // R-side
 os << "f " << (base + 8) << " " << (base + 5) << std::endl; // R-side
 os << "f " << (base + 1) << " " << (base + 5) << std::endl; // L/R-side
 os << "f " << (base + 2) << " " << (base + 6) << std::endl; // L/R-side
 os << "f " << (base + 3) << " " << (base + 7) << std::endl; // L/R-side
 os << "f " << (base + 4) << " " << (base + 8) << std::endl; // L/R-side
 os << std::endl;
 base += 8;
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

class boxtree {
 private :
  static const int n_bin = 8;
  static const int n_split = n_bin + 1;
 private :
  struct node {
   AABB_minmax volume;
   std::unique_ptr<node> children[8];
  };
  std::unique_ptr<node> root;
 public :
  void construct(const vector3D* verts, size_t n_verts, const unsigned int* faces, size_t n_indices);
  void clear();
 public :
  boxtree& operator =(const boxtree& other);
  boxtree& operator =(boxtree&& other);
 public :
  boxtree();
  boxtree(const boxtree& other);
  boxtree(boxtree&& other);
 ~boxtree();
};

int main()
{
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

 boxtree tree;
 tree.construct(points, 24, facelist, 24);
 
 return -1;
}

boxtree::boxtree()
{
}

boxtree::boxtree(const boxtree& other)
{
}

boxtree::boxtree(boxtree&& other)
{
}

boxtree::~boxtree()
{
}

boxtree& boxtree::operator =(const boxtree& other)
{
 if(this == &other) return *this;
 return *this;
}

boxtree& boxtree::operator =(boxtree&& other)
{
 if(this == &other) return *this;
 return *this;
}

void boxtree::construct(const vector3D* verts, size_t n_verts, const unsigned int* faces, size_t n_indices)
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
 int vb_base = 0;
 if(debug) {
    ofile.open("debug.obj");
    ofile << "o debug.obj" << endl;
   }

 //
 // PHASE #1
 // CONSTRUCT PER-FACE DATA
 // For each triangle, compute its centroid and its min-max AABB. The centroid
 // is used to place triangles into bins and the AABBs are used "loosen" boxes
 // so that they enclose all of their binned triangles.
 //

 // temporary per-face data
 std::unique_ptr<vector3D[]> min_face(new vector3D[n_faces]);
 std::unique_ptr<vector3D[]> max_face(new vector3D[n_faces]);
 std::unique_ptr<vector3D[]> centroid(new vector3D[n_faces]);

 // compute per-face data
 size_t vindex = 0;
 for(size_t i = 0; i < n_faces; i++)
    {
     // centroids
     vector3D v1 = verts[faces[vindex++]];
     vector3D v2 = verts[faces[vindex++]];
     vector3D v3 = verts[faces[vindex++]];
     centroid[i] = triangle_centroid(v1, v2, v3);

     // AABBs (compare v1)
     min_face[i][0] = max_face[i][0] = v1[0];
     min_face[i][1] = max_face[i][1] = v1[1];
     min_face[i][2] = max_face[i][2] = v1[2];
     // compare v2
     if(v2[0] < min_face[i][0]) min_face[i][0] = v2[0]; else if(max_face[i][0] < v2[0]) max_face[i][0] = v2[0];
     if(v2[1] < min_face[i][1]) min_face[i][1] = v2[1]; else if(max_face[i][1] < v2[1]) max_face[i][1] = v2[1];
     if(v2[2] < min_face[i][2]) min_face[i][2] = v2[2]; else if(max_face[i][2] < v2[2]) max_face[i][2] = v2[2];
     // compare v3
     if(v3[0] < min_face[i][0]) min_face[i][0] = v3[0]; else if(max_face[i][0] < v3[0]) max_face[i][0] = v3[0];
     if(v3[1] < min_face[i][1]) min_face[i][1] = v3[1]; else if(max_face[i][1] < v3[1]) max_face[i][1] = v3[1];
     if(v3[2] < min_face[i][2]) min_face[i][2] = v3[2]; else if(max_face[i][2] < v3[2]) max_face[i][2] = v3[2];
    }

 // debug
 if(debug)
   {
    // output triangle vertices
    for(size_t i = 0; i < n_verts; i++)
        ofile << "v " << verts[i][0] << " " << verts[i][1] << " " << -verts[i][2] << endl;

    // output triangle faces
    vindex = 0;
    for(size_t i = 0; i < n_faces; i++) {
        int f1 = 1 + faces[vindex++];
        int f2 = 1 + faces[vindex++];
        int f3 = 1 + faces[vindex++];
        ofile << "f " << f1 << " " << f2 << " " << f3 << endl;
       }
    vb_base += n_verts;

    // output centroids
    for(size_t i = 0; i < n_faces; i++)
        ofile << "v " << centroid[i][0] << " " << centroid[i][1] << " " << -centroid[i][2] << endl;        
    vb_base += n_faces;
   }

 //
 // PHASE #2
 // DEFINE ROOT NODE MIN-MAX AABB
 //

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
 root->volume.a[0] = min_b[0];
 root->volume.a[1] = min_b[1];
 root->volume.a[2] = min_b[2];
 root->volume.b[0] = max_b[0];
 root->volume.b[1] = max_b[1];
 root->volume.b[2] = max_b[2];
 if(debug) StreamToOBJ(ofile, root->volume, vb_base);

/*

 //
 // PHASE #3
 // SUBDIVIDE INTO OCTREE
 //

 // loop to split
 node* curr = root.get();
 // for(;;)
    {
     //
     // PHASE #3A
     // DIVIDE EACH AXIS INTO "X" NUMBER OF EVENLY-SPACED BINS
     //

     // split[0] = <1.3000, 0.1, 1.3> // AABB min
     // split[1] = <2.1875, 0.4, 2.1>
     // split[2] = <3.0750, 0.7, 2.9>
     // split[3] = <3.9625, 1.0, 3.7>
     // split[4] = <4.8500, 1.3, 4.5>
     // split[5] = <5.7375, 1.6, 5.3>
     // split[6] = <6.6250, 1.9, 6.1>
     // split[7] = <7.5125, 2.2, 6.9>
     // split[8] = <8.4000, 2.5, 7.7> // AABB max

     // AABB properties
     const float dx = (curr->volume.b[0] - curr->volume.a[0])/n_bin;
     const float dy = (curr->volume.b[1] - curr->volume.a[1])/n_bin;
     const float dz = (curr->volume.b[2] - curr->volume.a[2])/n_bin;

     // define split intervals
     float split_v[n_split][3];
     for(int i = 0; i < n_split; i++) {
         split_v[i][0] = curr->volume.a[0] + i*dx;
         split_v[i][1] = curr->volume.a[1] + i*dy;
         split_v[i][2] = curr->volume.a[2] + i*dz;
        }

     // keep track of exact bounds for each bin
     // later on, when we count triangles in each bin, these values will be adjusted
     float min_bin[n_bin][3];
     float max_bin[n_bin][3];
     for(int i = 0; i < n_bin; i++) {
         // min extents
         min_bin[i][0] = split_v[i][0];
         min_bin[i][1] = split_v[i][1];
         min_bin[i][2] = split_v[i][2];
         // max extents
         max_bin[i][0] = split_v[i + 1][0];
         max_bin[i][1] = split_v[i + 1][1];
         max_bin[i][2] = split_v[i + 1][2];
        }

     //
     // PHASE #3B
     // COUNT NUMBER OF TRIANGLES IN EACH BIN
     // When a triangle is placed in a bin, make sure to check to see if this
     // triangle crosses over into another bin. If it does, we need to extend
     // the bin's dimensions.
     //

     // keep track of number of faces in each bin
     int bin[n_bin][3];
     for(size_t i = 0; i < n_bin; i++) bin[i][0] = bin[i][1] = bin[i][2] = 0;

     // partition face centroids into bins
     for(size_t i = 0; i < n_faces; i++)
        {
         // split x-axis
         for(int j = 0; j < n_bin; j++) {
             float a = split_v[j][0];
             float b = split_v[j + 1][0];
             if(centroid[i][0] >= a && centroid[i][0] <= b) {
                bin[j][0]++;
                if(min_face[i][0] < min_bin[j][0]) min_bin[j][0] = min_face[i][0];
                if(max_face[i][0] > max_bin[j][0]) max_bin[j][0] = max_face[i][0];
                break;
               }
            }
         // split y-axis
         for(int j = 0; j < n_bin; j++) {
             float a = split_v[j][1];
             float b = split_v[j + 1][1];
             if(centroid[i][1] >= a && centroid[i][1] <= b) {
                bin[j][1]++;
                if(min_face[i][1] < min_bin[j][1]) min_bin[j][1] = min_face[i][1];
                if(max_face[i][1] > max_bin[j][1]) max_bin[j][1] = max_face[i][1];
                break;
               }
            }
         // split z-axis
         for(int j = 0; j < n_bin; j++) {
             float a = split_v[j][2];
             float b = split_v[j + 1][2];
             if(centroid[i][2] >= a && centroid[i][2] <= b) {
                bin[j][2]++;
                if(min_face[i][2] < min_bin[j][2]) min_bin[j][2] = min_face[i][2];
                if(max_face[i][2] > max_bin[j][2]) max_bin[j][2] = max_face[i][2];
                break;
               }
            }
        }

     //
     // PHASE #3C
     // DETERMINE BEST SPLIT FOR EACH AXIS
     //

     // BIN    [0]       [1]       [2]       [3]       [4]       [5]       [6]       [7]
     // L = <2, 1, 0> <3, 1, 2> <4, 3, 3> <5, 6, 5> <6, 7, 6> <6, 7, 7> <7, 7, 7> <8, 8, 8>
     // R = <6, 7, 8> <5, 7, 6> <4, 5, 5> <3, 2, 3> <2, 1, 2> <2, 1, 1> <1, 1, 1> <0, 0, 0>
     // D = <4, 6, 8> <2, 6, 4> <0, 2, 2> <2, 4, 2> <4, 6, 4> <4, 6, 6> <6, 6, 6> <8, 8, 8>
     // B =                      X  Y  Z

     // determine best axes split
     int L_count[3] = { bin[0][0], bin[0][1], bin[0][2] };
     int R_count[3] = { 
      n_faces - bin[0][0],
      n_faces - bin[0][1],
      n_faces - bin[0][2],
     };
     std::cout << "L_count[0] = " << L_count[0] << ", " << L_count[1] << ", " << L_count[2] << std::endl;
     std::cout << "R_count[0] = " << R_count[0] << ", " << R_count[1] << ", " << R_count[2] << std::endl;
     std::cout << std::endl;

     int best_d[3] = {
      std::abs(R_count[0] - L_count[0]),
      std::abs(R_count[1] - L_count[1]),
      std::abs(R_count[2] - L_count[2]),
     };
     int best_i[3] = { 0, 0, 0 };

     for(int i = 1; i < n_bin; i++) {
         for(int j = 0; j < 3; j++) {
             L_count[j] += bin[i][j];
             R_count[j] -= bin[i][j];
             int difference = std::abs(R_count[j] - L_count[j]);
             if(difference < best_d[j]) {
                best_d[j] = difference;
                best_i[j] = i;
               }
            }
         std::cout << "L_count[" << i << "] = " << L_count[0] << ", " << L_count[1] << ", " << L_count[2] << std::endl;
         std::cout << "R_count[" << i << "] = " << R_count[0] << ", " << R_count[1] << ", " << R_count[2] << std::endl;
         std::cout << std::endl;
        }
     std::cout << "best_D = " << std::endl;
     std::cout << best_d[0] << ", " << best_d[1] << ", " << best_d[2] << std::endl;
     std::cout << "best_I (bin index) = " << std::endl;
     std::cout << best_i[0] << ", " << best_i[1] << ", " << best_i[2] << std::endl;

     // x[min], y[min], z[min] refer to min_face[0][X], min_face[0][Y], and min_face[0][Z]
     // x[max], y[max], z[max] refer to max_face[0][X], max_face[0][Y], and max_face[0][Z]
     // x[mid], y[mid], z[mid] refer to
     //   min_face[best_i[X]], min_face[best_i[Y]], and min_face[best_i[Z]] for R-side
     //   max_face[best_i[X]], max_face[best_i[Y]], and max_face[best_i[Z]] for L-side
     //   x[min] to x[mid] is L-side (for example)
     //   x[mid] to x[max] is R-side (for example)

     // child[0]: R L R
     // <x[mid], y[min], z[mid]>
     // <x[max], y[mid], z[max]>
     AABB_minmax child;
     child.a[0] = min_bin[best_i[0]][0];
     child.a[1] = min_bin[0][1];
     child.a[2] = min_bin[best_i[2]][2];
     child.b[0] = max_bin[n_bin - 1][0];
     child.b[1] = max_bin[best_i[1]][1];
     child.b[2] = max_bin[n_bin - 1][2];
     if(debug) StreamToOBJ(ofile, child, vb_base);


     // now that we have our split point, we can partition
     // the AABB into eight parts
     float split_point[3] = {
      split_v[best_i[0] + 1][0],
      split_v[best_i[1] + 1][1],
      split_v[best_i[2] + 1][2],
     };

     std::cout << "split point = " << std::endl;
     std::cout << split_point[0] << ", " << split_point[1] << ", " << split_point[2] << std::endl;

     //
     if(debug) {
        // split X-axis
        float X[4][3] = {
         {
          split_point[0],    // split x
          curr->volume.b[1], // +y
          curr->volume.a[2], // -z
         },
         {
          split_point[0],    // split x
          curr->volume.a[1], // -y
          curr->volume.a[2], // -z
         },
         {
          split_point[0],    // split x
          curr->volume.a[1], // -y
          curr->volume.b[2], // +z
         },
         {
          split_point[0],    // split x
          curr->volume.b[1], // +y
          curr->volume.b[2], // +z
         },
        };
        ofile << "v " << X[0][0] << " " << X[0][1] << " " << -X[0][2] << endl;
        ofile << "v " << X[1][0] << " " << X[1][1] << " " << -X[1][2] << endl;
        ofile << "v " << X[2][0] << " " << X[2][1] << " " << -X[2][2] << endl;
        ofile << "v " << X[3][0] << " " << X[3][1] << " " << -X[3][2] << endl;
        int start = vb_base + 1;
        ofile << "f " << (start + 0) << " " << (start + 1) << " " << (start + 2) << " " << (start + 3) << endl;
        vb_base += 4;

        // split Y-axis
        float Y[4][3] = {
         {
          curr->volume.b[0], // +x
          split_point[1],    // split y
          curr->volume.a[2], // -z
         },
         {
          curr->volume.a[0], // -x
          split_point[1], // split y
          curr->volume.a[2], // -z
         },
         {
          curr->volume.a[0], // -x
          split_point[1], // split y
          curr->volume.b[2], // +z
         },
         {
          curr->volume.b[0], // +x
          split_point[1], // split y
          curr->volume.b[2], // +z
         },
        };
        ofile << "v " << Y[0][0] << " " << Y[0][1] << " " << -Y[0][2] << endl;
        ofile << "v " << Y[1][0] << " " << Y[1][1] << " " << -Y[1][2] << endl;
        ofile << "v " << Y[2][0] << " " << Y[2][1] << " " << -Y[2][2] << endl;
        ofile << "v " << Y[3][0] << " " << Y[3][1] << " " << -Y[3][2] << endl;
        start = vb_base + 1;
        ofile << "f " << (start + 0) << " " << (start + 1) << " " << (start + 2) << " " << (start + 3) << endl;
        vb_base += 4;

        // split Z-axis
        float p[4][3] = {
         {
          curr->volume.b[0], // +x
          curr->volume.a[1], // -y
          split_point[2]     // split z
         },
         {
          curr->volume.a[0], // -x
          curr->volume.a[1], // -y
          split_point[2]     // split z
         },
         {
          curr->volume.a[0], // -x
          curr->volume.b[1], // +y
          split_point[2]     // split z
         },
         {
          curr->volume.b[0], // +x
          curr->volume.b[1], // +y
          split_point[2]     // split z
         },
        };
        ofile << "v " << p[0][0] << " " << p[0][1] << " " << -p[0][2] << endl;
        ofile << "v " << p[1][0] << " " << p[1][1] << " " << -p[1][2] << endl;
        ofile << "v " << p[2][0] << " " << p[2][1] << " " << -p[2][2] << endl;
        ofile << "v " << p[3][0] << " " << p[3][1] << " " << -p[3][2] << endl;
        start = vb_base + 1;
        ofile << "f " << (start + 0) << " " << (start + 1) << " " << (start + 2) << " " << (start + 3) << endl;
        vb_base += 4;
       }
    }
*/
}

void boxtree::clear()
{
}