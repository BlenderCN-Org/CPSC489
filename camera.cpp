#include "stdafx.h"
#include "stdgfx.h"
#include "errors.h"
#include "math.h"
#include "vector3.h"
#include "matrix4x4.h"
#include "ray.h"
#include "gfx.h"
#include "camera.h"

// Orbit Camera Variables
static OrbitCamera orbitcam;
OrbitCamera* GetOrbitCamera(void) { return &orbitcam; }

#pragma region CONSTRUCTORS_DESTRUCTORS

OrbitCamera::OrbitCamera()
{
 // Viewport Properties
 viewport[0] = 0;
 viewport[1] = 0;
 viewport[2] = 1;
 viewport[3] = 1;
 aspect = 1.0f;

 // Frustum Variables
 Reset();
}

OrbitCamera::OrbitCamera(const OrbitCamera& vc)
{
 // viewport settings
 viewport[0] = vc.viewport[0];
 viewport[1] = vc.viewport[1];
 viewport[2] = vc.viewport[2];
 viewport[3] = vc.viewport[3];
 aspect = vc.aspect;
}

OrbitCamera::~OrbitCamera()
{
}

#pragma endregion CONSTRUCTORS_DESTRUCTORS

#pragma region VIEWPORT_FUNCTIONS

void OrbitCamera::SetViewport(const int* vp)
{
 SetViewport(vp[0], vp[1], vp[2], vp[3]);
}

void OrbitCamera::SetViewport(int x0, int y0, int dx, int dy)
{
 // set viewport
 viewport[0] = x0;
 viewport[1] = y0;
 viewport[2] = (dx < 1 ? 1 : dx);
 viewport[3] = (dy < 1 ? 1 : dy);

 // set aspect ratio
 aspect = (real32)dx/(real32)dy;
}

#pragma endregion VIEWPORT_FUNCTIONS

#pragma region RESET_FUNCTIONS

void OrbitCamera::Reset(void)
{
 // frustum variables
 nplane = 0.1000f;
 fplane = 1000.0f;
 fovy = 30.0f;

 // camera variables
 cam_E[0] = 0.0f;
 cam_E[1] = 0.0f;
 cam_E[2] = 1.0f;
 POSITIVE_X_AXIS(cam_X);
 POSITIVE_Y_AXIS(cam_Y);
 POSITIVE_Z_AXIS(cam_Z);

 // orbit variables
 orbit_distance = 1.0f;
 horz_angle = 0.0f;
 vert_angle = 0.0f;
 POSITIVE_X_AXIS(ref_X);
 POSITIVE_Y_AXIS(ref_Y);
 POSITIVE_Z_AXIS(ref_Z);
}

#pragma endregion RESET_FUNCTIONS

#pragma region FRUSTUM_FUNCTIONS

real32 OrbitCamera::GetNearPlane(void)const
{
 return nplane;
}

real32 OrbitCamera::GetFarPlane(void)const
{
 return fplane;
}

std::pair<real32, real32> OrbitCamera::GetFrustumPlanes(void)const
{
 return std::make_pair(nplane, fplane);
}

bool OrbitCamera::SetNearPlane(real32 n)
{
 if((n < 1.0e-4f) || (n >= fplane)) return false;
 nplane = n;
 return true;
}

bool OrbitCamera::SetFarPlane(real32 f)
{
 if(nplane >= f) return false;
 fplane = f;
 return true;
}

bool OrbitCamera::SetFrustumPlanes(real32 n, real32 f)
{
 if((n < 1.0e-4f) || (n >= f)) return false;
 nplane = n;
 fplane = f;
 return true;
}

bool OrbitCamera::FrustumPlanesValid(void)const
{
 return ((!(nplane < 1.0e-4f)) && (nplane < fplane));
}

bool OrbitCamera::GetClippingPlaneCoords(real32* coords)const
{
 if(!coords || !FrustumPlanesValid()) return false;
 real32 hy = nplane*std::abs(std::tan(radians(fovy/2.0f)));
 real32 hx = hy*aspect;
 coords[0] = -hx;
 coords[1] =  hx;
 coords[2] = -hy;
 coords[3] =  hy;
 return true;
}

#pragma endregion FRUSTUM_FUNCTIONS

#pragma region FOVY_FUNCTIONS

real32 OrbitCamera::GetFOVY(void)const
{
 return fovy;
}

bool OrbitCamera::SetFOVY(real32 value)
{
 if(value < 1.0f || value > 179.0f) return false;
 fovy = value;
 return true;
}

#pragma endregion FOVY_FUNCTIONS

#pragma region CAMERA_FUNCTIONS_PRIVATE

void OrbitCamera::SetCameraOrigin(real32 x, real32 y, real32 z)
{
 cam_E[0] = x;
 cam_E[1] = y;
 cam_E[2] = z;
}

void OrbitCamera::SetCameraXAxis(real32 x, real32 y, real32 z)
{
 cam_X[0] = x;
 cam_X[1] = y;
 cam_X[2] = z;
}

void OrbitCamera::SetCameraYAxis(real32 x, real32 y, real32 z)
{
 cam_Y[0] = x;
 cam_Y[1] = y;
 cam_Y[2] = z;
}

void OrbitCamera::SetCameraZAxis(real32 x, real32 y, real32 z)
{
 cam_Z[0] = x;
 cam_Z[1] = y;
 cam_Z[2] = z;
}

void OrbitCamera::SetCameraOrigin(const real32* origin)
{
 SetCameraOrigin(origin[0], origin[1], origin[2]);
}

void OrbitCamera::SetCameraXAxis(const real32* axis)
{
 SetCameraXAxis(axis[0], axis[1], axis[2]);
}

void OrbitCamera::SetCameraYAxis(const real32* axis)
{
 SetCameraYAxis(axis[0], axis[1], axis[2]);
}

void OrbitCamera::SetCameraZAxis(const real32* axis)
{
 SetCameraZAxis(axis[0], axis[1], axis[2]);
}

#pragma endregion CAMERA_FUNCTIONS_PRIVATE

#pragma region SCREEN_SPACE_FUNCTIONS

bool OrbitCamera::ScreenToPlane(const int* p, real32 distance, real32* v)const
{
 return ScreenToPlane(p[0], p[1], distance, v);
}

bool OrbitCamera::ScreenToPlane(int x, int y, real32 distance, real32* v)const
{
 // compute center point (world coordinates)
 real32 center[3] = {
  cam_E[0] + cam_X[0]*distance,
  cam_E[1] + cam_X[1]*distance,
  cam_E[2] + cam_X[2]*distance
 };

 // compute center point (normalized screen coordinates)
 real32 px = ((real32)x - (real32)viewport[0])/((real32)viewport[2]);
 real32 py = ((real32)y - (real32)viewport[1])/((real32)viewport[3]);
 
 // adjust to [-0.5, +0.5]
 px -= 0.5f;
 py -= 0.5f;

 // half-widths (world coordinates)
 real32 hy = distance*std::abs(std::tan(radians(fovy/2.0f)));
 real32 hx = hy*GetAspectRatio();
    
 // normalized screen coordinates to world coordinates
 px *= (hx + hx);
 py *= (hy + hy);
 
 // adjust from center
 // remember, left-axis is +y-axis, so use -px
 v[0] = center[0] - px*cam_Y[0] - py*cam_Z[0];
 v[1] = center[1] - px*cam_Y[1] - py*cam_Z[1];
 v[2] = center[2] - px*cam_Y[2] - py*cam_Z[2];

 return true;
}

bool OrbitCamera::ScreenToNearPlane(const int* p, real32* v)const
{
 return ScreenToPlane(p, nplane, v);
}

bool OrbitCamera::ScreenToNearPlane(int x, int y, real32* v)const
{
 return ScreenToPlane(x, y, nplane, v);
}

bool OrbitCamera::ScreenToFarPlane(const int* p, real32* v)const
{
 return ScreenToPlane(p, fplane, v);
}

bool OrbitCamera::ScreenToFarPlane(int x, int y, real32* v)const
{
 return ScreenToPlane(x, y, fplane, v);
}

bool OrbitCamera::ScreenToGroundPlane(const int* p, real32* v)const
{
 return ScreenToGroundPlane(p[0], p[1], v);
}

bool OrbitCamera::ScreenToGroundPlane(int x, int y, real32* v)const
{
 // get point on near plane that was clicked on
 real32 point[3];
 if(!ScreenToPlane(x, y, nplane, point)) return false;

 // compute vector from camera origin to point on near plane
 point[0] -= cam_E[0];
 point[1] -= cam_E[1];
 point[2] -= cam_E[2];
 vector3D_normalize(point);
    
 // intersect with ground plane?
 const real32 plane[4] = { 0.0f, 0.0f, 1.0f, 0.0f };
 real32 result[3];
 real32 distance = ray3D_plane_intersect_distance(result, cam_E, point, plane);
 if(distance < 0.0000f) return false; // opposite direction
 if(distance > 1000.0f) return false; // too far
    
 // success
 v[0] = result[0];
 v[1] = result[1];
 v[2] = 0.0f; // z should always be zero

 return true;
}

#pragma endregion SCREEN_SPACE_FUNCTIONS

#pragma region MOUSE_TRACKING_FUNCTIONS

bool OrbitCamera::Pan(int x0, int y0, int x1, int y1)
{
 // compute points on plane
 real32 p0[3] = { 0.0f, 0.0f, 0.0f };
 real32 p1[3] = { 0.0f, 0.0f, 0.0f };
 ScreenToPlane(x0, y0, orbit_distance, p0);
 ScreenToPlane(x1, y1, orbit_distance, p1);
    
 // compute drag axis from these points
 real32 drag_axis[3];
 drag_axis[0] = p1[0] - p0[0];
 drag_axis[1] = p1[1] - p0[1];
 drag_axis[2] = p1[2] - p0[2];
    
 // compute drag length
 real32 drag_length = std::sqrt(drag_axis[0]*drag_axis[0] + drag_axis[1]*drag_axis[1] + drag_axis[2]*drag_axis[2]);
 if(!drag_length) return false;

 // normalize drag axis
 real32 scale = 1.0f/drag_length;
 drag_axis[0] *= scale;
 drag_axis[1] *= scale;
 drag_axis[2] *= scale;
    
 // adjust camera position
 cam_E[0] -= drag_length*drag_axis[0];
 cam_E[1] -= drag_length*drag_axis[1];
 cam_E[2] -= drag_length*drag_axis[2];

 return true;
}

bool OrbitCamera::Pan(int dx, int dy)
{
 int x0 = viewport[0] + viewport[2]/2; // center of viewport
 int y0 = viewport[1] + viewport[3]/2; // center of viewport
 int x1 = x0 + dx;             // off-center
 int y1 = y0 + dy;             // off-center
 return Pan(x0, y0, x1, y1);
}

bool OrbitCamera::Orbit(int x0, int y0, int x1, int y1)
{
 // nothing to do
 int mouse_dx = x1 - x0;
 int mouse_dy = y1 - y0;
 if(!mouse_dx && !mouse_dy) return true;

 // add to global angles (how much we are going to turn our turntable)
 real32 mdx = (real32)mouse_dx;
 real32 mdy = (real32)mouse_dy;
 horz_angle += radians(0.5f*mdx*-1.0f);
 vert_angle += radians(0.5f*mdy);
    
 // clamp vertical rotation to (-90, +90) degrees (avoid poles)
 const real32 max_vert = radians(89.9999f);
 if(vert_angle < -max_vert) vert_angle = -max_vert;
 else if(vert_angle > max_vert) vert_angle = max_vert;
    
 // rotation matrices
 real32 R1[16];
 real32 R2[16];
 matrix4D_rotate(R1, ref_Y, vert_angle);
 matrix4D_rotate(R2, ref_Z, horz_angle);
    
 // multiplied transforms
 real32 R[16];
 matrix4D_mul(R, R2, R1); // z 1st and y 2nd
    
 // compute orbit point origin
 real32 orbit_point[3];
 orbit_point[0] = cam_E[0] + orbit_distance*cam_X[0];
 orbit_point[1] = cam_E[1] + orbit_distance*cam_X[1];
 orbit_point[2] = cam_E[2] + orbit_distance*cam_X[2];
    
 // compute new camera axes
 matrix4D_vector3_mul(cam_X, R, ref_X);
 matrix4D_vector3_mul(cam_Y, R, ref_Y);
 matrix4D_vector3_mul(cam_Z, R, ref_Z);
    
 // compute new camera origin
 cam_E[0] = orbit_point[0] - orbit_distance*cam_X[0];
 cam_E[1] = orbit_point[1] - orbit_distance*cam_X[1];
 cam_E[2] = orbit_point[2] - orbit_distance*cam_X[2];

 return true;
}

bool OrbitCamera::Dolly(int unit)
{
 // clamp unit to 100%
 // dolly by a percentage of the orbit distance so that when the distance is large,
 // we don't take FOREVER to dolly into whatever we are looking at
 if(!unit) return false;
 else if(unit < -100) unit = -100;
 else if(unit > +100) unit = +100;

 // minimum and maximum orbit distances
 real32 min_orbit = nplane;
 real32 max_orbit = std::abs(fplane);

 // translate camera
 real32 value = orbit_distance + (real32)unit*orbit_distance/100.0f;
 if(value < min_orbit) value = min_orbit;
 else if(value > max_orbit) value = max_orbit;

 // adjust orbit distance
 real32 delta = value - orbit_distance;
 cam_E[0] -= delta*cam_X[0];
 cam_E[1] -= delta*cam_X[1];
 cam_E[2] -= delta*cam_X[2];
 orbit_distance += delta;

 return true;
}

#pragma endregion MOUSE_TRACKING_FUNCTIONS