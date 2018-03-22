#include<iostream>
using namespace std;

struct QUATERNION {
 float q[4];
};
struct POINT {
 float v[3];
};
struct JOINT {
 unsigned int parent;
 float m_abs[16];
 float m_inv[16];
 float m_rel[16];
};
static const int n_jnts = 3;
static JOINT jntdata[n_jnts];
static QUATERNION transforms[3];
static POINT point;
static POINT point_transformed;
static POINT point_blender;
static POINT joint_blender[3];

//
// REQUIREMENTS
//


//
//
//
void LoadSample(void);

int main()
{
 LoadSample();
 return -1;
}

void LoadSample(void)
{
 // joint[0]
 jntdata[0].parent = 0xFFFFFFFFul;
 jntdata[0].m_abs[0x0] = 1.0f;
 jntdata[0].m_abs[0x1] = 0.0f;
 jntdata[0].m_abs[0x2] = 0.0f;
 jntdata[0].m_abs[0x3] = 1.0f;
 jntdata[0].m_abs[0x4] = 0.0f;
 jntdata[0].m_abs[0x5] = 0.4999999701976776f;
 jntdata[0].m_abs[0x6] = -0.866025447845459f;
 jntdata[0].m_abs[0x7] = 2.0f;
 jntdata[0].m_abs[0x8] = 0.0f;
 jntdata[0].m_abs[0x9] = 0.866025447845459f;
 jntdata[0].m_abs[0xA] = 0.4999999701976776f;
 jntdata[0].m_abs[0xB] = 3.0f;
 jntdata[0].m_abs[0xC] = 0.0f;
 jntdata[0].m_abs[0xD] = 0.0f;
 jntdata[0].m_abs[0xE] = 0.0f;
 jntdata[0].m_abs[0xF] = 1.0f;

 // joint[1]
 jntdata[0].parent = 0;
 jntdata[0].m_abs[0x0] = 1.0f;
 jntdata[0].m_abs[0x1] = 0.0f;
 jntdata[0].m_abs[0x2] = 0.0f;
 jntdata[0].m_abs[0x3] = 1.0f;
 jntdata[0].m_abs[0x4] = 0.0f;
 jntdata[0].m_abs[0x5] = 0.9551945924758911f;
 jntdata[0].m_abs[0x6] = -0.29597848653793335f;
 jntdata[0].m_abs[0x7] = 2.5f;
 jntdata[0].m_abs[0x8] = 0.0f;
 jntdata[0].m_abs[0x9] = 0.29597848653793335f;
 jntdata[0].m_abs[0xA] = 0.9551945924758911f;
 jntdata[0].m_abs[0xB] = 3.866025447845459f;
 jntdata[0].m_abs[0xC] = 0.0f;
 jntdata[0].m_abs[0xD] = 0.0f;
 jntdata[0].m_abs[0xE] = 0.0f;
 jntdata[0].m_abs[0xF] = 1.0f;

 // joint[2]
 jntdata[2].parent = 1;
 jntdata[2].m_abs[0x0] = 0.9999998211860657f;
 jntdata[2].m_abs[0x1] = 0.0f;
 jntdata[2].m_abs[0x2] = 0.000640869140625f;
 jntdata[2].m_abs[0x3] = 1.0f;
 jntdata[2].m_abs[0x4] = -0.00032257664133794606f;
 jntdata[2].m_abs[0x5] = 0.8640871047973633f;
 jntdata[2].m_abs[0x6] = 0.5033423900604248f;
 jntdata[2].m_abs[0x7] = 3.181468963623047f;
 jntdata[2].m_abs[0x8] = -0.0005537667311728001f;
 jntdata[2].m_abs[0x9] = -0.5033424496650696f;
 jntdata[2].m_abs[0xA] = 0.8640868663787842f;
 jntdata[2].m_abs[0xB] = 4.077186584472656f;
 jntdata[2].m_abs[0xC] = 0.0f;
 jntdata[2].m_abs[0xD] = 0.0f;
 jntdata[2].m_abs[0xE] = 0.0f;
 jntdata[2].m_abs[0xF] = 1.0f;

 // transform #1
 transforms[0].q[0] = 0.845863f;
 transforms[0].q[1] = 0.007272f;
 transforms[0].q[2] = -0.491491f;
 transforms[0].q[3] = -0.207121f;

 // transform #2
 transforms[1].q[0] = 0.490048f;
 transforms[1].q[1] = 0.256793f;
 transforms[1].q[2] = 0.686258f;
 transforms[1].q[3] = 0.472187f;

 // transform #3
 transforms[2].q[0] = 0.443239f;
 transforms[2].q[1] = 0.819386f;
 transforms[2].q[2] = -0.271397f;
 transforms[2].q[3] = 0.241845f;

 // point to transform
 point.v[0] = 1.25f;
 point.v[1] = 3.50f;
 point.v[2] = 4.00f;

 // point transformed in Blender
 point_blender.v[0] = 1.61264f;
 point_blender.v[1] = 2.16104f;
 point_blender.v[2] = 4.69789f;

 // joint transformed in Blender
 joint_blender[0].v[0] = 1.0f;
 joint_blender[0].v[1] = 2.0f;
 joint_blender[0].v[2] = 3.0f;

 joint_blender[1].v[0] = 1.34324f;
 joint_blender[1].v[1] = 2.27007f;
 joint_blender[1].v[2] = 3.89958f;

 joint_blender[2].v[0] = 1.31243f;
 joint_blender[2].v[1] = 2.42429f;
 joint_blender[2].v[2] = 4.59546f;
}