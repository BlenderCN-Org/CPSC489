#include<iostream>
#include<fstream>
#include<vector>
#include<deque>
#include<cmath>
#include<memory>

#include "aabb.h"

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
  static const int n_bins = 8;
  static const int n_split = n_bins + 1;
 private :
  struct node {
   AABB_minmax volume;
   std::unique_ptr<node> L;
   std::unique_ptr<node> R;
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

 // per-bin data
 AABB_minmax binlist[n_bins];
 unsigned int bintris[n_bins];

 //
 // PHASE #1
 // CONSTRUCT PER-FACE DATA
 // For each triangle, compute its min-max AABB and its centroid. The centroid
 // is used to place triangles into bins and the AABBs are used "loosen" boxes
 // so that they enclose all of their binned triangles.
 //

 // temporary per-face data
 std::unique_ptr<AABB_minmax[]> blist(new AABB_minmax[n_faces]);
 std::unique_ptr<vector3D[]> clist(new vector3D[n_faces]);

 // compute per-face data
 size_t vindex = 0;
 for(size_t i = 0; i < n_faces; i++) {
     vector3D v1 = verts[faces[vindex++]];
     vector3D v2 = verts[faces[vindex++]];
     vector3D v3 = verts[faces[vindex++]];
     blist[i].from(v1.v, v2.v, v3.v);
     centroid(blist[i], clist[i].v);
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
        ofile << "v " << clist[i][0] << " " << clist[i][1] << " " << -clist[i][2] << endl;        
    vb_base += n_faces;

    // output boxes
    for(size_t i = 0; i < n_faces; i++)
        StreamToOBJ(ofile, blist[i], vb_base);
   }

 //
 // PHASE #2
 // NON-RECURSIVE SPACIAL PARITIONING
 //

 // AABB tree
 struct AABB_node {
  AABB_minmax aabb;
  unsigned int params[2];
 };
 std::vector<AABB_node> tree;

 // add root node
 tree.push_back(AABB_node());

 // add root node to process stack
 struct BVHSTACKITEM {
  unsigned int tree_index;
  unsigned int face_index[2];
 };
 std::deque<BVHSTACKITEM> stack;
 stack.push_front(BVHSTACKITEM());
 stack.front().tree_index = 0;
 stack.front().face_index[0] = 0;
 stack.front().face_index[1] = n_faces;

 // non-recursive partitioning
 while(stack.size())
      {
       //
       // STEP #1
       // OBTAIN PARTITION
       //

       // get and pop item from stack
       BVHSTACKITEM item = stack.front();
       unsigned int tree_index = item.tree_index;
       unsigned int face_index[2] = { item.face_index[0], item.face_index[1] };
       stack.pop_front();

       //
       // STEP #2
       // CALCULATE PARTITION BOUNDS
       //

       // per-partition data (all triangle bounds, all centroid bounds)
       AABB_minmax tbounds(blist[face_index[0]]);
       AABB_minmax cbounds(clist[face_index[0]]);
       for(unsigned int i = (face_index[0] + 1); i < face_index[1]; i++) {
           tbounds.grow(blist[i]);
           cbounds.grow(clist[i]);
          }

       // debug: display triangle and centroid bounds
       if(debug) {
          StreamToOBJ(ofile, tbounds, vb_base);
          StreamToOBJ(ofile, cbounds, vb_base);
         }

       //
       // STEP #3
       // DOMINANT AXIS
       //

       // choose dominant axis from "longest-axis" of centroid bounds
       float dv[3]; // dx, dy, dz saved from computing dominator
       unsigned int axis = cbounds.dominator(dv);

       // dominant axis is too small, make this is a leaf node
       // 1.0e-6f is just a suggestion, but it can be made bigger, like 0.01f or something
       if(dv[axis] < 1.0e-6f) {
          tree[tree_index].aabb = tbounds;
          tree[tree_index].params[0] = face_index[0] | 0x80000000ul; // mark as a leaf node
          tree[tree_index].params[1] = face_index[1] - face_index[0];
          continue;
         }

       // print centroid bounds
       std::cout << "CENTROID BOUNDS" << std::endl;
       std::cout << "bounds[x] = " << dv[0] << std::endl;
       std::cout << "bounds[y] = " << dv[1] << std::endl;
       std::cout << "bounds[z] = " << dv[2] << std::endl;
       if(axis == 0) std::cout << "dominant axis is X-axis" << std::endl;
       if(axis == 1) std::cout << "dominant axis is Y-axis" << std::endl;
       if(axis == 2) std::cout << "dominant axis is Z-axis" << std::endl;
       std::cout << std::endl;

       // print bin planes
       std::cout << "BIN PLANES" << std::endl;
       std::cout << "min plane = " << cbounds.a[axis] << std::endl;
       std::cout << "max plane = " << cbounds.b[axis] << std::endl;
       const float bin_dv = dv[axis]/n_bins;
       for(size_t i = 0; i < (n_bins + 1); i++) std::cout << "plane[ " << i << "] = " << (cbounds.a[axis] + i*bin_dv) << std::endl;
       std::cout << std::endl;

       //
       // STEP #4
       // COMPUTE BINS
       //

       // start = cbounds.a[axis] + i*(dv[axis]/n_bins)
       // i = (start - cbounds.a[axis])/(dv[axis]/n_bins)
       // i = n_bins*(start - cbounds.a[axis])/dv[axis]
       // let k0 = cbounds.a[axis]
       // let k1 = n_bins/dv[axis]
       // i = k1*(start - k0)

       //   0     1     2     3     4     5     6     7     8
       // 1.7 - 2.5 - 3.3 - 4.1 - 4.9 - 5.7 - 6.5 - 7.3 - 8.1
       // dv[x] = (8.1 - 1.7) = 6.4
       // k1 = 8*1/6.4 = 1.25 (1.24999875 with epsilon)

       // for example given, start = 3.0
       // i = k1*(3.0 - k0)
       // i = (1.25)*(3.0 - 1.7) = 1.625

       // for example given, start = 3.3
       // i = k1*(3.3 - k0)
       // i = (1.25)*(3.3 - 1.7) = 2.0

       // so what the epsilon does is that if anything is on the edge
       // of the next bin, like for example when start = 8.1, it will
       // place that triangle in the bin before it

       // binning constants
       const float k0 = cbounds.a[axis];
       const float k1 = n_bins*(1.0f - 1.0e-6f)/dv[axis];

       // initialize per-bin data
       for(unsigned int i = 0; i < n_bins; i++) {
           bintris[i] = 0;
           //binlist[i].from(MAXVALUE, MINVALUE);
          }
      }

/*
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
 //if(debug) StreamToOBJ(ofile, root->volume, vb_base);

 // determine most-dominant axis
 float dv[3] = {
  root->volume.b[0] - root->volume.a[0],
  root->volume.b[1] - root->volume.a[1],
  root->volume.b[2] - root->volume.a[2]
 };
 int dominant = 0;
 if(dv[0] < dv[1]) {
    if(dv[1] < dv[2]) dominant = 2;
    else dominant = 1;
   }
 else if(dv[0] < dv[2])
    dominant = 2;

 std::cout << "DOMINANT AXIS:" << std::endl;
 std::cout << "dv[x] = " << dv[0] << std::endl;
 std::cout << "dv[y] = " << dv[1] << std::endl;
 std::cout << "dv[z] = " << dv[2] << std::endl;
 std::cout << std::endl;

 //
 // PHASE #3
 // SUBDIVIDE
 //

 // loop to split
 node* curr = root.get();
 // for(;;)
    {
     //
     // PHASE #3A
     // DIVIDE DOMINANT AXIS INTO "X" NUMBER OF EVENLY-SPACED BINS
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

     // define split intervals
     const float bin_dv = dv[dominant]/n_bin;
     float split_v[n_split];
     for(int i = 0; i < n_split; i++) {
         split_v[i] = curr->volume.a[dominant] + i*bin_dv;
         std::cout << split_v[i] << std::endl;
        }

     //
     // PHASE #3B
     // COUNT NUMBER OF TRIANGLES IN EACH BIN
     // When a triangle is placed in a bin, make sure to check to see if this
     // triangle crosses over into another bin. If it does, we need to extend
     // the bin's dimensions.
     //

     // keep track of number of faces in each bin
     int bin[n_bin];
     for(size_t i = 0; i < n_bin; i++) bin[i] = 0;

     // partition face centroids into bins
     for(size_t i = 0; i < n_faces; i++) {
         // split dominant axis
         for(int j = 0; j < n_bin; j++) {
             float a = split_v[j];
             float b = split_v[j + 1];
             if(centroid[i][dominant] >= a && centroid[i][dominant] <= b) {
                bin[j]++;
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
     int L_count = bin[0];
     int R_count = n_faces - bin[0];
     std::cout << "SPLITTING INTO BINS" << std::endl;
     std::cout << "L_count[i] = " << L_count << std::endl;
     std::cout << "R_count[i] = " << R_count << std::endl;

     int best_d = std::abs(R_count - L_count);
     int best_i = 0;
     for(int i = 1; i < n_bin; i++) {
         L_count += bin[i];
         R_count -= bin[i];
         int difference = std::abs(R_count - L_count);
         if(difference < best_d) {
            best_d = difference;
            best_i = i;
           }
         std::cout << "L_count[" << i << "] = " << L_count << std::endl;
         std::cout << "R_count[" << i << "] = " << R_count << std::endl;
        }
     std::cout << "best_D = " << best_d << std::endl;
     std::cout << "best_I (bin index) = " << best_i << std::endl;
     std::cout << std::endl;

     // keep track of exact bounds for each bin
     // later on, when we count triangles in each bin, these values will be adjusted
     float L_min_bin[3];
     float L_max_bin[3];
     L_min_bin[0] = max_b[0]; L_min_bin[1] = max_b[1]; L_min_bin[2] = max_b[2];
     L_max_bin[0] = min_b[0]; L_max_bin[1] = min_b[1]; L_max_bin[2] = min_b[2];

     // partition face centroids into bins
     for(size_t i = 0; i < n_faces; i++)
        {
         float a = split_v[0];
         float b = split_v[best_i + 1];
         if(centroid[i][dominant] >= a && centroid[i][dominant] <= b) {
            if(min_face[i][0] < L_min_bin[0]) L_min_bin[0] = min_face[i][0]; if(max_face[i][0] > L_max_bin[0]) L_max_bin[0] = max_face[i][0];
            if(min_face[i][1] < L_min_bin[1]) L_min_bin[1] = min_face[i][1]; if(max_face[i][1] > L_max_bin[1]) L_max_bin[1] = max_face[i][1];
            if(min_face[i][2] < L_min_bin[2]) L_min_bin[2] = min_face[i][2]; if(max_face[i][2] > L_max_bin[2]) L_max_bin[2] = max_face[i][2];
           }
        }

     // child[L]:
     // <x[min], y[min], z[min]> one axis will be min
     // <x[max], y[max], z[max]>                  mid
     AABB_minmax L;
     if(dominant == 0) {
        L.a[0] = L_min_bin[0];
        L.a[1] = L_min_bin[1];
        L.a[2] = L_min_bin[2];
        L.b[0] = L_max_bin[0];
        L.b[1] = L_max_bin[1];
        L.b[2] = L_max_bin[2];
       }
     if(debug) StreamToOBJ(ofile, L, vb_base);

     AABB_minmax R;

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