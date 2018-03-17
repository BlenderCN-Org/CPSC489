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

class boxtree {
 private :
  static const int n_bins = 8;
  static const int n_part = n_bins - 1;
 private :
  struct node {
   AABB_minmax volume;
   std::unique_ptr<node> L;
   std::unique_ptr<node> R;
  };
  std::unique_ptr<node> root;
 public :
  void construct(const vector3D* verts, size_t n_verts, unsigned int* faces, size_t n_indices);
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

void boxtree::construct(const vector3D* verts, size_t n_verts, unsigned int* faces, size_t n_indices)
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

 // per-partition data
 float costs[n_part];
 unsigned int NL[n_part];
 unsigned int NR[n_part];
 AABB_minmax BL[n_part];
 AABB_minmax BR[n_part];

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
          //StreamToOBJ(ofile, tbounds, vb_base);
          //StreamToOBJ(ofile, cbounds, vb_base);
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
           binlist[i].from(std::numeric_limits<float>::max(), std::numeric_limits<float>::lowest());
          }

       // count number of triangles in bins and grow the bin AABBs to fit the triangles that are in each bin
       for(unsigned int i = face_index[0]; i < face_index[1]; i++) {
           unsigned int binindex = static_cast<unsigned int>(k1*(clist[i][axis] - k0));
           bintris[binindex]++;
           binlist[binindex].grow(blist[i]);
          }

       //
       // STEP #5
       // PARTITIONING
       //

       //  bin:   0     1     2     3     4     5     6     7     8
       // axis: 1.7 - 2.5 - 3.3 - 4.1 - 4.9 - 5.7 - 6.5 - 7.3 - 8.1

       // there are n_bins - 1 cases we need to think about
       // L part[0]: bin 0         - AND - R part[0]: bin 1 - bin 7
       // L part[1]: bin 0 - bin 1 - AND - R part[1]: bin 2 - bin 7
       // L part[2]: bin 0 - bin 2 - AND - R part[2]: bin 3 - bin 7
       // L part[3]: bin 0 - bin 3 - AND - R part[3]: bin 4 - bin 7
       // L part[4]: bin 0 - bin 4 - AND - R part[4]: bin 5 - bin 7
       // L part[5]: bin 0 - bin 5 - AND - R part[5]: bin 6 - bin 7
       // L part[6]: bin 0 - bin 6 - AND - R part[6]: bin 7

       // L-side computation
       NL[0] = bintris[0];
       BL[0] = binlist[0];
       for(unsigned int i = 1; i < n_part; i++) {
           NL[i] = NL[i - 1] + bintris[i];
           BL[i].from(BL[i - 1], binlist[i]);
          }

       // R-side computation
       unsigned int j = n_part - 1;
       unsigned int k = n_part;
       NR[j] = bintris[k];
       BR[j] = binlist[k];
       for(unsigned int i = 1; i < n_part; i++) {
           j--;
           k--;           
           NR[j] = NR[k] + bintris[k];
           BR[j].from(BR[k], binlist[k]);
          }

       // if unrolling loops and hardcoding bins
       // NR[6] = bintris[7];
       // BR[6] = binlist[7];
       // NR[5] = NR[6] + bintris[6]; BR[5].from(BR[6], binlist[6]);
       // NR[4] = NR[5] + bintris[5]; BR[4].from(BR[5], binlist[5]);
       // NR[3] = NR[4] + bintris[4]; BR[3].from(BR[4], binlist[4]);
       // NR[2] = NR[3] + bintris[3]; BR[2].from(BR[3], binlist[3]);
       // NR[1] = NR[2] + bintris[2]; BR[1].from(BR[2], binlist[2]);
       // NR[0] = NR[1] + bintris[1]; BR[0].from(BR[1], binlist[1]);

       // compute index of best partition
       unsigned int best_index = 0;
       costs[0] = NL[0]*half_surface_area(BL[0]) + NR[0]*half_surface_area(BR[0]);
       for(unsigned int i = 1; i < n_part; i++) {
           costs[i] = NL[i]*half_surface_area(BL[i]) + NR[i]*half_surface_area(BR[i]);
           if(costs[i] < costs[best_index]) best_index = i;
          }

       std::cout << "SURFACE AREA HEURISTIC INFORMATION" << std::endl;
       std::cout << "best index = " << best_index << std::endl;
       for(size_t i = 0; i < n_part; i++) {
           std::cout << "PARTITION[" << i << "]" << std::endl;
           float AL = half_surface_area(BL[i]);
           float AR = half_surface_area(BR[i]);
           std::cout << "NL[" << i << "] = " << NL[i] << std::endl;
           std::cout << "NR[" << i << "] = " << NR[i] << std::endl;
           std::cout << "BL[" << i << "] = " << BL[i] << std::endl;
           std::cout << "BR[" << i << "] = " << BR[i] << std::endl;
           std::cout << "AL[" << i << "] = " << AL << std::endl;
           std::cout << "AR[" << i << "] = " << AR << std::endl;
           std::cout << "costs[" << i << "] = " << costs[i] << std::endl;
           std::cout << std::endl;
          }

       if(debug) {
          StreamToOBJ(ofile, BL[best_index], vb_base);
          StreamToOBJ(ofile, BR[best_index], vb_base);
         }

       //
       // STEP #6
       // INDEX BUFFER SORTING
       //

       auto BIN_INDEX_FROM_FACE = [&](unsigned int face) { return static_cast<unsigned int>(k1*(clist[face][axis] - k0)); };

       // display before sorting
       if(debug) {
          std::cout << "PRE-SORT ASSIGNMENT" << std::endl;
          for(unsigned int i = face_index[0]; i < face_index[1]; i++) {
              unsigned int binindex = BIN_INDEX_FROM_FACE(i);
              std::cout << "face[" << i << "] is in bin " << binindex << " and is on ";
              if(best_index < binindex) std::cout << "R subtree." << std::endl;
              else std::cout << "L subtree." << std::endl;
             }
          std::cout << std::endl;
         }

       // sort index buffer
       unsigned int L_count = 0;
       unsigned int L_pivot = face_index[0];
       unsigned int R_pivot = face_index[1] - 1;

       // stop when L_count matches with the number of faces on the L-side
       while(L_count < NL[best_index])
            {
             // bin for L_pivot is on R-side
             unsigned int i = BIN_INDEX_FROM_FACE(L_pivot);
             if(best_index < i)
               {
                for(;;)
                   {
                    // bin for R_pivot is on R-side (just move R_pivot over)
                    unsigned int j = BIN_INDEX_FROM_FACE(R_pivot);
                    if(best_index < j) R_pivot--;
                    // bin for R_pivot is on L-side
                    else
                      {
                       // swap faces
                       unsigned int L_face = 3*L_pivot;
                       unsigned int R_face = 3*R_pivot;
                       std::swap(faces[L_face++], faces[R_face++]);
                       std::swap(faces[L_face++], faces[R_face++]);
                       std::swap(faces[L_face], faces[R_face]);
                       // swap per-face centroids
                       std::swap(clist[L_pivot][0], clist[R_pivot][0]);
                       std::swap(clist[L_pivot][1], clist[R_pivot][1]);
                       std::swap(clist[L_pivot][2], clist[R_pivot][2]);
                       // swap per-face AABBs
                       std::swap(blist[L_pivot], blist[R_pivot]);
                       // move pivots
                       L_pivot++;
                       R_pivot--;
                       L_count++;
                       break;
                      }
                   }
               }
             // bin for L_pivot is on L-side (just move L_pivot over)
             else {
                L_pivot++;
                L_count++;
               }
            }

       // display after sorting
       if(debug) {
          std::cout << "POST-SORT ASSIGNMENT" << std::endl;
          for(unsigned int i = face_index[0]; i < face_index[1]; i++) {
              unsigned int binindex = BIN_INDEX_FROM_FACE(i);
              std::cout << "face[" << i << "] is in bin " << binindex << " and is on ";
              if(best_index < binindex) std::cout << "R subtree." << std::endl;
              else std::cout << "L subtree." << std::endl;
             }
          std::cout << std::endl;
         }

       //
       // STEP #7
       // DIVIDE AND CONQUER
       //

       auto BIN_LIMIT_TEST = [](unsigned int a) { return (a < 2ul); };

       // this node
       tree[tree_index].aabb = tbounds;
       tree[tree_index].params[0] = tree.size();
       tree[tree_index].params[1] = tree.size() + 1;

       // L = leaf
       tree.push_back(AABB_node());
       if(BIN_LIMIT_TEST(NL[best_index])) {
          tree.back().params[0] = (face_index[0] | 0x80000000ul);
          tree.back().params[1] = NL[best_index];
          tree.back().aabb.from(BL[best_index]);
         }
       // L = node
       else {
          stack.push_front(BVHSTACKITEM());
          stack.front().tree_index = tree[tree_index].params[0];
          stack.front().face_index[0] = face_index[0];
          stack.front().face_index[1] = face_index[0] + L_count;
         }

         // R = leaf
         tree.push_back(AABB_node());
         if(BIN_LIMIT_TEST(NR[best_index])) {
            unsigned int pivot = face_index[0] + L_count;
            tree.back().params[0] = (pivot | 0x80000000ul);
            tree.back().params[1] = NR[best_index];
            tree.back().aabb.from(BR[best_index]);
           }
         // R = node
         else {
            stack.push_front(BVHSTACKITEM());
            stack.front().tree_index = tree[tree_index].params[1];
            stack.front().face_index[0] = face_index[0] + L_count;
            stack.front().face_index[1] = face_index[1];
           }
      }
}

void boxtree::clear()
{
}