#include<shared/stdafx.h>
#include<shared/ascii.h>
#include<shared/vector3.h>
#include<shared/matrix4.h>
using namespace std;

static const int n_verts = 125;
static const int n_faces = 128;
static float vb[n_verts][5];
static int ib[n_faces][3];

struct MeshUTFJoint {
 STDSTRINGW name;
 uint32 parent;
 real32 position[3];
 matrix4D m;
 matrix4D m_abs;
};
static std::vector<MeshUTFJoint> joints;
static std::vector<uint32> childcount;

//
struct VERTEX {
 float position[3];
 float uv[2]; 
};
struct FACE {
 uint16 data[3];
};
static vector<VERTEX> vdata;
static vector<FACE> fdata;
static uint32 vbase = 0;

void CreateBoneData(void);
void SaveBoneData(void);
bool LoadArmature(const wchar_t* filename);

int main()
{
 CreateBoneData();
 SaveBoneData();
 if(!LoadArmature(L"enemy.txt")) {
    cout << "Failed to load armature." << endl;
    return -1;
   }
 return -1;
}

void CreateBoneData(void)
{
 // copy point data
 float vb1[n_verts][3] = {
  {  0.0500f, -0.0866f,  0.0000f },
  {  0.0433f, -0.0866f, -0.0250f },
  {  0.0250f, -0.0866f, -0.0433f },
  {  0.0000f, -0.0866f, -0.0500f },
  { -0.0250f, -0.0866f, -0.0433f },
  { -0.0433f, -0.0866f, -0.0250f },
  { -0.0433f, -0.0866f,  0.0250f },
  { -0.0250f, -0.0866f,  0.0433f },
  {  0.0000f, -0.0866f,  0.0500f },
  {  0.0250f, -0.0866f,  0.0433f },
  {  0.0433f, -0.0866f,  0.0250f },
  {  0.0866f, -0.0500f,  0.0000f },
  {  0.0750f, -0.0500f, -0.0433f },
  {  0.0433f, -0.0500f, -0.0750f },
  {  0.0000f, -0.0500f, -0.0866f },
  { -0.0433f, -0.0500f, -0.0750f },
  { -0.0750f, -0.0500f, -0.0433f },
  { -0.0750f, -0.0500f,  0.0433f },
  { -0.0433f, -0.0500f,  0.0750f },
  {  0.0000f, -0.0500f,  0.0866f },
  {  0.0433f, -0.0500f,  0.0750f },
  {  0.0750f, -0.0500f,  0.0433f },
  {  0.1000f,  0.0000f,  0.0000f },
  {  0.0866f,  0.0000f, -0.0500f },
  {  0.0500f,  0.0000f, -0.0866f },
  {  0.0000f,  0.0000f, -0.1000f },
  { -0.0500f,  0.0000f, -0.0866f },
  { -0.0866f,  0.0000f, -0.0500f },
  { -0.0866f,  0.0000f,  0.0500f },
  { -0.0500f,  0.0000f,  0.0866f },
  {  0.0000f,  0.0000f,  0.1000f },
  {  0.0500f,  0.0000f,  0.0866f },
  {  0.0866f,  0.0000f,  0.0500f },
  {  0.0866f,  0.0500f,  0.0000f },
  {  0.0750f,  0.0500f, -0.0433f },
  {  0.0433f,  0.0500f, -0.0750f },
  {  0.0000f,  0.0500f, -0.0866f },
  { -0.0433f,  0.0500f, -0.0750f },
  { -0.0750f,  0.0500f, -0.0433f },
  { -0.0750f,  0.0500f,  0.0433f },
  { -0.0433f,  0.0500f,  0.0750f },
  {  0.0000f,  0.0500f,  0.0866f },
  {  0.0433f,  0.0500f,  0.0750f },
  {  0.0750f,  0.0500f,  0.0433f },
  {  0.0500f,  0.0866f,  0.0000f },
  {  0.0433f,  0.0866f, -0.0250f },
  {  0.0250f,  0.0866f, -0.0433f },
  {  0.0000f,  0.0866f, -0.0500f },
  { -0.0250f,  0.0866f, -0.0433f },
  { -0.0433f,  0.0866f, -0.0250f },
  { -0.0433f,  0.0866f,  0.0250f },
  { -0.0250f,  0.0866f,  0.0433f },
  {  0.0000f,  0.0866f,  0.0500f },
  {  0.0250f,  0.0866f,  0.0433f },
  {  0.0433f,  0.0866f,  0.0250f },
  {  0.0000f, -0.1000f,  0.0000f },
  {  0.0000f, -0.1000f,  0.0000f },
  {  0.0000f, -0.1000f,  0.0000f },
  {  0.0000f, -0.1000f,  0.0000f },
  {  0.0000f, -0.1000f,  0.0000f },
  {  0.0000f, -0.1000f,  0.0000f },
  { -0.0500f, -0.0866f,  0.0000f },
  {  0.0000f, -0.1000f,  0.0000f },
  { -0.0500f, -0.0866f,  0.0000f },
  {  0.0000f, -0.1000f,  0.0000f },
  {  0.0000f, -0.1000f,  0.0000f },
  {  0.0000f, -0.1000f,  0.0000f },
  {  0.0000f, -0.1000f,  0.0000f },
  {  0.0000f, -0.1000f,  0.0000f },
  { -0.0500f, -0.0866f,  0.0000f },
  { -0.0866f, -0.0500f,  0.0000f },
  { -0.0866f, -0.0500f,  0.0000f },
  { -0.0500f, -0.0866f,  0.0000f },
  { -0.0866f, -0.0500f,  0.0000f },
  { -0.0866f, -0.0500f,  0.0000f },
  { -0.0866f, -0.0500f,  0.0000f },
  { -0.1000f,  0.0000f,  0.0000f },
  { -0.1000f,  0.0000f,  0.0000f },
  { -0.0866f, -0.0500f,  0.0000f },
  { -0.1000f,  0.0000f,  0.0000f },
  { -0.1000f,  0.0000f,  0.0000f },
  { -0.1000f,  0.0000f,  0.0000f },
  { -0.1000f,  0.0000f,  0.0000f },
  { -0.0866f,  0.0500f,  0.0000f },
  { -0.1000f,  0.0000f,  0.0000f },
  { -0.1000f,  0.0000f,  0.0000f },
  { -0.0866f,  0.0500f,  0.0000f },
  { -0.0866f,  0.0500f,  0.0000f },
  { -0.0866f,  0.0500f,  0.0000f },
  { -0.0500f,  0.0866f,  0.0000f },
  { -0.0866f,  0.0500f,  0.0000f },
  { -0.0866f,  0.0500f,  0.0000f },
  { -0.0500f,  0.0866f,  0.0000f },
  {  0.0000f,  0.1000f,  0.0000f },
  {  0.0000f,  0.1000f,  0.0000f },
  {  0.0000f,  0.1000f,  0.0000f },
  {  0.0000f,  0.1000f,  0.0000f },
  {  0.0000f,  0.1000f,  0.0000f },
  { -0.0500f,  0.0866f,  0.0000f },
  {  0.0000f,  0.1000f,  0.0000f },
  { -0.0500f,  0.0866f,  0.0000f },
  {  0.0000f,  0.1000f,  0.0000f },
  {  0.0000f,  0.1000f,  0.0000f },
  {  0.0000f,  0.1000f,  0.0000f },
  {  0.0000f,  0.1000f,  0.0000f },
  {  0.0000f,  0.1000f,  0.0000f },
  {  0.0000f,  0.1000f,  0.0000f },
  {  0.0000f,  0.0000f, -1.0000f },
  {  0.0000f,  0.0000f,  0.0000f },
  { -0.1250f, -0.1250f, -0.1250f },
  { -0.1250f,  0.1250f, -0.1250f },
  {  0.1250f,  0.1250f, -0.1250f },
  { -0.1250f,  0.1250f, -0.1250f },
  {  0.1250f, -0.1250f, -0.1250f },
  {  0.1250f,  0.1250f, -0.1250f },
  { -0.1250f,  0.1250f, -0.1250f },
  {  0.1250f,  0.1250f, -0.1250f },
  {  0.1250f, -0.1250f, -0.1250f },
  {  0.1250f,  0.1250f, -0.1250f },
  {  0.1250f, -0.1250f, -0.1250f },
  { -0.1250f, -0.1250f, -0.1250f },
  { -0.1250f, -0.1250f, -0.1250f },
  { -0.1250f,  0.1250f, -0.1250f },
  { -0.1250f, -0.1250f, -0.1250f },
  {  0.1250f, -0.1250f, -0.1250f },
 };
 float vb2[100][2] = {
  { 0.013824f, 0.089976f },
  { 0.013824f, 0.169776f },
  { 0.013824f, 0.249576f },
  { 0.013824f, 0.329376f },
  { 0.013824f, 0.409176f },
  { 0.033774f, 0.010176f },
  { 0.033774f, 0.488976f },
  { 0.053724f, 0.089976f },
  { 0.053724f, 0.169776f },
  { 0.053724f, 0.249576f },
  { 0.053724f, 0.329376f },
  { 0.053724f, 0.409176f },
  { 0.073674f, 0.010176f },
  { 0.073674f, 0.488976f },
  { 0.093624f, 0.089976f },
  { 0.093624f, 0.169776f },
  { 0.093624f, 0.249576f },
  { 0.093624f, 0.329376f },
  { 0.093624f, 0.409176f },
  { 0.113574f, 0.010176f },
  { 0.113574f, 0.488976f },
  { 0.133524f, 0.089976f },
  { 0.133524f, 0.169776f },
  { 0.133524f, 0.249576f },
  { 0.133524f, 0.329376f },
  { 0.133524f, 0.409176f },
  { 0.153474f, 0.010176f },
  { 0.153474f, 0.488976f },
  { 0.173424f, 0.089976f },
  { 0.173424f, 0.169776f },
  { 0.173424f, 0.249576f },
  { 0.173424f, 0.329376f },
  { 0.173424f, 0.409176f },
  { 0.193374f, 0.010176f },
  { 0.193374f, 0.488976f },
  { 0.213324f, 0.089976f },
  { 0.213324f, 0.169776f },
  { 0.213324f, 0.249576f },
  { 0.213324f, 0.329376f },
  { 0.213324f, 0.409176f },
  { 0.233274f, 0.010176f },
  { 0.233274f, 0.488976f },
  { 0.253224f, 0.089976f },
  { 0.253224f, 0.169776f },
  { 0.253224f, 0.249576f },
  { 0.253224f, 0.329376f },
  { 0.253224f, 0.409176f },
  { 0.273174f, 0.010176f },
  { 0.273174f, 0.488976f },
  { 0.293124f, 0.089976f },
  { 0.293124f, 0.169776f },
  { 0.293124f, 0.249576f },
  { 0.293124f, 0.329376f },
  { 0.293124f, 0.409176f },
  { 0.313074f, 0.010176f },
  { 0.313074f, 0.488976f },
  { 0.333024f, 0.089976f },
  { 0.333024f, 0.169776f },
  { 0.333024f, 0.249576f },
  { 0.333024f, 0.329376f },
  { 0.333024f, 0.409176f },
  { 0.352974f, 0.010176f },
  { 0.352974f, 0.488976f },
  { 0.372924f, 0.089976f },
  { 0.372924f, 0.169776f },
  { 0.372924f, 0.249576f },
  { 0.372924f, 0.329376f },
  { 0.372924f, 0.409176f },
  { 0.392874f, 0.010176f },
  { 0.392874f, 0.488976f },
  { 0.412824f, 0.089976f },
  { 0.412824f, 0.169776f },
  { 0.412824f, 0.249576f },
  { 0.412824f, 0.329376f },
  { 0.412824f, 0.409176f },
  { 0.432774f, 0.010176f },
  { 0.432774f, 0.488976f },
  { 0.452724f, 0.089976f },
  { 0.452724f, 0.169776f },
  { 0.452724f, 0.249576f },
  { 0.452724f, 0.329376f },
  { 0.452724f, 0.409176f },
  { 0.472674f, 0.010176f },
  { 0.472674f, 0.488976f },
  { 0.492624f, 0.089976f },
  { 0.492624f, 0.169776f },
  { 0.492624f, 0.249576f },
  { 0.492624f, 0.329376f },
  { 0.492624f, 0.409176f },
  { 0.493676f, 0.569720f },
  { 0.609623f, 0.520123f },
  { 0.677794f, 0.352530f },
  { 0.677794f, 0.479049f },
  { 0.735600f, 0.974835f },
  { 0.736000f, 0.502500f },
  { 0.741053f, 0.415789f },
  { 0.804312f, 0.352530f },
  { 0.804312f, 0.479049f },
  { 0.863443f, 0.519719f },
  { 0.980905f, 0.570125f },
 };
 int ib1[n_faces][3] = {
  {  2,   1,  56 },
  {  3,   2,  57 },
  {  4,   3,  58 },
  {  5,   4,  59 },
  {  6,   5,  60 },
  { 62,   6,  61 },
  {  7,  64,  63 },
  {  8,   7,  65 },
  {  9,   8,  66 },
  { 10,   9,  67 },
  { 11,  10,  68 },
  {  1,  11,  69 },
  {  2,  12,   1 },
  { 13,  12,   2 },
  { 14,  13,   2 },
  {  3,  14,   2 },
  { 15,  14,   3 },
  {  4,  15,   3 },
  { 15,   4,   5 },
  { 16,  15,   5 },
  { 17,   5,   6 },
  { 17,  16,   5 },
  { 71,   6,  70 },
  { 72,  17,   6 },
  { 74,  73,   7 },
  { 18,  75,   7 },
  { 19,   7,   8 },
  { 19,  18,   7 },
  { 20,   8,   9 },
  { 20,  19,   8 },
  { 10,  20,   9 },
  { 21,  20,  10 },
  { 11,  21,  10 },
  { 22,  21,  11 },
  { 12,  22,  11 },
  {  1,  12,  11 },
  { 13,  23,  12 },
  { 24,  23,  13 },
  { 14,  24,  13 },
  { 25,  24,  14 },
  { 26,  25,  14 },
  { 15,  26,  14 },
  { 26,  15,  16 },
  { 27,  26,  16 },
  { 27,  16,  17 },
  { 28,  27,  17 },
  { 77,  17,  76 },
  { 78,  28,  17 },
  { 80,  79,  18 },
  { 29,  81,  18 },
  { 30,  18,  19 },
  { 30,  29,  18 },
  { 31,  19,  20 },
  { 31,  30,  19 },
  { 21,  31,  20 },
  { 32,  31,  21 },
  { 22,  32,  21 },
  { 33,  32,  22 },
  { 23,  33,  22 },
  { 12,  23,  22 },
  { 35,  34,  23 },
  { 24,  35,  23 },
  { 25,  35,  24 },
  { 36,  35,  25 },
  { 26,  36,  25 },
  { 37,  36,  26 },
  { 38,  26,  27 },
  { 38,  37,  26 },
  { 39,  27,  28 },
  { 39,  38,  27 },
  { 39,  28,  82 },
  { 84,  39,  83 },
  { 40,  85,  29 },
  { 40,  87,  86 },
  { 40,  29,  30 },
  { 41,  40,  30 },
  { 41,  30,  31 },
  { 42,  41,  31 },
  { 43,  42,  31 },
  { 32,  43,  31 },
  { 33,  43,  32 },
  { 44,  43,  33 },
  { 23,  44,  33 },
  { 34,  44,  23 },
  { 46,  45,  34 },
  { 35,  46,  34 },
  { 36,  46,  35 },
  { 47,  46,  36 },
  { 37,  47,  36 },
  { 48,  47,  37 },
  { 49,  37,  38 },
  { 49,  48,  37 },
  { 50,  38,  39 },
  { 50,  49,  38 },
  { 50,  39,  88 },
  { 90,  50,  89 },
  { 51,  91,  40 },
  { 51,  93,  92 },
  { 52,  40,  41 },
  { 52,  51,  40 },
  { 52,  41,  42 },
  { 53,  52,  42 },
  { 54,  53,  42 },
  { 43,  54,  42 },
  { 44,  54,  43 },
  { 55,  54,  44 },
  { 34,  55,  44 },
  { 45,  55,  34 },
  { 46,  94,  45 },
  { 47,  95,  46 },
  { 48,  96,  47 },
  { 97,  48,  49 },
  { 98,  49,  50 },
  {100,  50,  99 },
  {102, 101,  51 },
  {103,  51,  52 },
  {104,  52,  53 },
  { 54, 105,  53 },
  { 55, 106,  54 },
  { 45, 107,  55 },
  {111, 108, 110 },
  {113, 112, 108 },
  {108, 115, 114 },
  {117, 116, 109 },
  {119, 109, 118 },
  {109, 121, 120 },
  {109, 123, 122 },
  {108, 125, 124 },
 };
 int ib2[n_faces][3] = {
  { 50,  43,  48 },
  { 57,  50,  55 },
  { 64,  57,  62 },
  { 71,  64,  69 },
  { 78,  71,  76 },
  { 85,  78,  83 },
  {  8,   1,   6 },
  { 15,   8,  13 },
  { 22,  15,  20 },
  { 29,  22,  27 },
  { 36,  29,  34 },
  { 43,  36,  41 },
  { 50,  44,  43 },
  { 51,  44,  50 },
  { 58,  51,  50 },
  { 57,  58,  50 },
  { 65,  58,  57 },
  { 64,  65,  57 },
  { 65,  64,  71 },
  { 72,  65,  71 },
  { 79,  71,  78 },
  { 79,  72,  71 },
  { 86,  78,  85 },
  { 86,  79,  78 },
  {  2,   1,   8 },
  {  9,   2,   8 },
  { 16,   8,  15 },
  { 16,   9,   8 },
  { 23,  15,  22 },
  { 23,  16,  15 },
  { 29,  23,  22 },
  { 30,  23,  29 },
  { 36,  30,  29 },
  { 37,  30,  36 },
  { 44,  37,  36 },
  { 43,  44,  36 },
  { 51,  45,  44 },
  { 52,  45,  51 },
  { 58,  52,  51 },
  { 59,  52,  58 },
  { 66,  59,  58 },
  { 65,  66,  58 },
  { 66,  65,  72 },
  { 73,  66,  72 },
  { 73,  72,  79 },
  { 80,  73,  79 },
  { 87,  79,  86 },
  { 87,  80,  79 },
  {  3,   2,   9 },
  { 10,   3,   9 },
  { 17,   9,  16 },
  { 17,  10,   9 },
  { 24,  16,  23 },
  { 24,  17,  16 },
  { 30,  24,  23 },
  { 31,  24,  30 },
  { 37,  31,  30 },
  { 38,  31,  37 },
  { 45,  38,  37 },
  { 44,  45,  37 },
  { 53,  46,  45 },
  { 52,  53,  45 },
  { 59,  53,  52 },
  { 60,  53,  59 },
  { 66,  60,  59 },
  { 67,  60,  66 },
  { 74,  66,  73 },
  { 74,  67,  66 },
  { 81,  73,  80 },
  { 81,  74,  73 },
  { 81,  80,  87 },
  { 88,  81,  87 },
  { 11,   3,  10 },
  { 11,   4,   3 },
  { 11,  10,  17 },
  { 18,  11,  17 },
  { 18,  17,  24 },
  { 25,  18,  24 },
  { 32,  25,  24 },
  { 31,  32,  24 },
  { 38,  32,  31 },
  { 39,  32,  38 },
  { 45,  39,  38 },
  { 46,  39,  45 },
  { 54,  47,  46 },
  { 53,  54,  46 },
  { 60,  54,  53 },
  { 61,  54,  60 },
  { 67,  61,  60 },
  { 68,  61,  67 },
  { 75,  67,  74 },
  { 75,  68,  67 },
  { 82,  74,  81 },
  { 82,  75,  74 },
  { 82,  81,  88 },
  { 89,  82,  88 },
  { 12,   4,  11 },
  { 12,   5,   4 },
  { 19,  11,  18 },
  { 19,  12,  11 },
  { 19,  18,  25 },
  { 26,  19,  25 },
  { 33,  26,  25 },
  { 32,  33,  25 },
  { 39,  33,  32 },
  { 40,  33,  39 },
  { 46,  40,  39 },
  { 47,  40,  46 },
  { 54,  49,  47 },
  { 61,  56,  54 },
  { 68,  63,  61 },
  { 70,  68,  75 },
  { 77,  75,  82 },
  { 84,  82,  89 },
  {  7,   5,  12 },
  { 14,  12,  19 },
  { 21,  19,  26 },
  { 33,  28,  26 },
  { 40,  35,  33 },
  { 47,  42,  40 },
  { 95,  94,  91 },
  { 95,  99,  94 },
  { 94,  99, 100 },
  { 98,  93,  96 },
  { 98,  96,  97 },
  { 96,  92,  97 },
  { 96,  93,  92 },
  { 94,  90,  91 },
 };

 // copy position data over first
 for(int i = 0; i < n_verts; i++) 
     for(int j = 0; j < 3; j++) ::vb[i][j] = vb1[i][j];

 // copy index buffer
 for(int i = 0; i < n_faces; i++)
     for(int j = 0; j < 3; j++) ::ib[i][j] = ib1[i][j] - 1;

 // copy over UVs
 for(int i = 0; i < n_faces; i++)
    {
     // vertex buffer refs
     int vb_a = ib1[i][0] - 1;
     int vb_b = ib1[i][1] - 1;
     int vb_c = ib1[i][2] - 1;

     // UV buffer refs
     int uv_a = ib2[i][0] - 1;
     int uv_b = ib2[i][1] - 1;
     int uv_c = ib2[i][2] - 1;

     // copy to vertex
     ::vb[vb_a][3] = vb2[uv_a][0]; ::vb[vb_a][4] = vb2[uv_a][1];
     ::vb[vb_b][3] = vb2[uv_b][0]; ::vb[vb_b][4] = vb2[uv_b][1];
     ::vb[vb_c][3] = vb2[uv_c][0]; ::vb[vb_c][4] = vb2[uv_c][1];
    }
}

void SaveBoneData(void)
{
 ofstream ofile("test.obj");
 ofile << "o test.obj" << endl;
 // verts
 for(int i = 0; i < n_verts; i++)
     ofile << "v " << vb[i][0] << " " << vb[i][1] << " " << vb[i][2] << endl;
 // UVs
 for(int i = 0; i < n_verts; i++)
     ofile << "vt " << vb[i][3] << " " << vb[i][4] << endl;
 // faces
 for(int i = 0; i < n_faces; i++)
     ofile << "f " << (ib[i][0] + 1) << "/" << (ib[i][0] + 1) << " "
                   << (ib[i][1] + 1) << "/" << (ib[i][1] + 1) << " "
                   << (ib[i][2] + 1) << "/" << (ib[i][2] + 1) << endl;
}

bool LoadArmature(const wchar_t* filename)
{
 // read file
 deque<string> linelist;
 if(!ASCIIParseFile(filename, linelist)) return false;

 // read number of joints
 uint32 n_jnts = 0;
 if(!ASCIIReadUint32(linelist, &n_jnts)) return false;
 if(!n_jnts) return false;

 // allocate joints
 joints.resize(n_jnts);
 childcount.resize(n_jnts, 0ul);

 // read joints
 for(uint32 i = 0; i < n_jnts; i++)
    {
     // read name
     char buffer[1024];
     if(!ASCIIReadString(linelist, buffer)) return false;

     // name must be valid
     joints[i].name = ConvertUTF8ToUTF16(buffer);
     if(!joints[i].name.length()) return false;

     // read parent
     if(!ASCIIReadUint32(linelist, &joints[i].parent)) return false;

     // set child count
     if(joints[i].parent != 0xFFFFFFFFul)
        childcount[joints[i].parent]++;

     // read position
     if(!ASCIIReadVector3(linelist, &joints[i].position[0], false)) return false;

     // read matrix
     if(!ASCIIReadMatrix4(linelist, &joints[i].m[0], false)) return false;

     // save matrix to absolute format
     joints[i].m_abs[0x0] = joints[i].m[0x0];
     joints[i].m_abs[0x1] = joints[i].m[0x1];
     joints[i].m_abs[0x2] = joints[i].m[0x2];
     joints[i].m_abs[0x3] = joints[i].position[0];
     joints[i].m_abs[0x4] = joints[i].m[0x4];
     joints[i].m_abs[0x5] = joints[i].m[0x5];
     joints[i].m_abs[0x6] = joints[i].m[0x6];
     joints[i].m_abs[0x7] = joints[i].position[1];
     joints[i].m_abs[0x8] = joints[i].m[0x8];
     joints[i].m_abs[0x9] = joints[i].m[0x9];
     joints[i].m_abs[0xA] = joints[i].m[0xA];
     joints[i].m_abs[0xB] = joints[i].position[2];
     joints[i].m_abs[0xC] = joints[i].m[0xC];
     joints[i].m_abs[0xD] = joints[i].m[0xD];
     joints[i].m_abs[0xE] = joints[i].m[0xE];
     joints[i].m_abs[0xF] = joints[i].m[0xF];
    }

 // clear previous data
 vdata.clear();
 fdata.clear();
 vbase = 0;

 ofstream ofile("output.obj");
 ofile << "o output.obj" << endl;
 ofile << endl;

 ofile << "mtllib output.mtl" << endl;

 ofstream mfile("output.mtl");
 for(uint32 i = 0; i < n_jnts; i++) {
     mfile << "newmtl " << ConvertUTF16ToUTF8(joints[i].name.c_str()).c_str() << endl;
     mfile << endl;
    } 

 // process joints
 for(uint32 i = 0; i < n_jnts; i++)
    {
     // do nothing for root
     uint32 parent = joints[i].parent;
     if(parent == 0xFFFFFFFFul) continue;

     // draw one bone from parent to child
     if(childcount[parent] == 1)
       {
        // copy vertices
        float copy[n_verts][5];
        for(uint32 v = 0; v < n_verts; v++) {
            copy[v][0] =  vb[v][0];
            copy[v][1] = -vb[v][2]; // swap y and z-axis and negate z-axis
            copy[v][2] =  vb[v][1]; // since lightwave is LHCS
            copy[v][3] =  vb[v][3];
            copy[v][4] =  vb[v][4];
           }

        // compute scaling factor
        float x1 = joints[parent].position[0];
        float y1 = joints[parent].position[1];
        float z1 = joints[parent].position[2];
        float x2 = joints[i].position[0];
        float y2 = joints[i].position[1];
        float z2 = joints[i].position[2];
        float scale = sqrt((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1) + (z2 - z1)*(z2 - z1));

        // compute scaling matrix
        matrix4D S;
        S.load_scaling(scale, scale, scale);

        // compute rotation matrix
        matrix4D R(joints[parent].m);

        // compute translation matrix
        matrix4D T;
        T.load_translation(joints[parent].position[0], joints[parent].position[1], joints[parent].position[2]); 
        
        // scale, then rotate, then translate
        matrix4D M = T*R*S;
        for(uint32 v = 0; v < n_verts; v++) {
            vector3D p(copy[v][0], copy[v][1], copy[v][2]);
            vector3D r = M*p;
            copy[v][0] = r[0];
            copy[v][1] = r[1];
            copy[v][2] = r[2];
           }

        // verts
        for(int v = 0; v < n_verts; v++)
            ofile << "v " << copy[v][0] << " " << copy[v][1] << " " << copy[v][2] << endl;
        // UVs
        for(int v = 0; v < n_verts; v++)
            ofile << "vt " << vb[v][3] << " " << vb[v][4] << endl;

        // faces
        ofile << "usemtl " << ConvertUTF16ToUTF8(joints[parent].name.c_str()).c_str() << endl;
        ofile << "g " << ConvertUTF16ToUTF8(joints[parent].name.c_str()).c_str() << endl;
        for(int f = 0; f < n_faces; f++)
            ofile << "f " << (vbase + ib[f][0] + 1) << "/" << (vbase + ib[f][0] + 1) << " "
                          << (vbase + ib[f][1] + 1) << "/" << (vbase + ib[f][1] + 1) << " "
                          << (vbase + ib[f][2] + 1) << "/" << (vbase + ib[f][2] + 1) << endl;

        vbase += n_verts;
       }
     // draw one line from parent to child
     else if(childcount[parent] > 1)
       {
        ofile << "v " << joints[parent].position[0] << " " << joints[parent].position[1] << " " << joints[parent].position[2] << endl;
        ofile << "v " << joints[i].position[0] << " " << joints[i].position[1] << " " << joints[i].position[2] << endl;
        ofile << "vt 0 0" << endl;
        ofile << "vt 1 1" << endl;
        ofile << "usemtl " << ConvertUTF16ToUTF8(joints[parent].name.c_str()).c_str() << endl;
        ofile << "g " << ConvertUTF16ToUTF8(joints[parent].name.c_str()).c_str() << endl;
        ofile << "f " << (vbase + 1) << "/" << (vbase + 1) << " " << (vbase + 2) << "/" << (vbase + 2) << endl;
        vbase += 2;
       }
    }
 
 return true;
}